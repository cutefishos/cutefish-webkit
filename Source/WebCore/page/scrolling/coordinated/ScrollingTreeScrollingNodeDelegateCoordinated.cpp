/*
 * Copyright (C) 2019 Apple Inc. All rights reserved.
 * Copyright (C) 2019, 2021, 2024 Igalia S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ScrollingTreeScrollingNodeDelegateCoordinated.h"

#if ENABLE(ASYNC_SCROLLING) && USE(COORDINATED_GRAPHICS)
#include "ScrollerCoordinated.h"
#include "ScrollingStateFrameScrollingNode.h"
#include "ScrollingTreeOverflowScrollingNode.h"

namespace WebCore {

ScrollingTreeScrollingNodeDelegateCoordinated::ScrollingTreeScrollingNodeDelegateCoordinated(ScrollingTreeScrollingNode& scrollingNode, bool scrollAnimatorEnabled)
    : ThreadedScrollingTreeScrollingNodeDelegate(scrollingNode)
    , m_scrollAnimatorEnabled(scrollAnimatorEnabled)
#if USE(COORDINATED_GRAPHICS_ASYNC_SCROLLBAR)
    , m_scrollerPair(ScrollerPairCoordinated::create(scrollingNode))
#endif
{
    ASSERT(isMainThread());
#if USE(COORDINATED_GRAPHICS_ASYNC_SCROLLBAR)
    if (is<ScrollingTreeOverflowScrollingNode>(scrollingNode) && ScrollbarTheme::theme().usesOverlayScrollbars()) {
        protect(m_scrollerPair->horizontalScroller())->setOverlayScrollbarEnabled(true);
        protect(m_scrollerPair->verticalScroller())->setOverlayScrollbarEnabled(true);
    }
#endif
}

ScrollingTreeScrollingNodeDelegateCoordinated::~ScrollingTreeScrollingNodeDelegateCoordinated() = default;

void ScrollingTreeScrollingNodeDelegateCoordinated::updateVisibleLengths()
{
    m_scrollController.contentsSizeChanged();
#if USE(COORDINATED_GRAPHICS_ASYNC_SCROLLBAR)
    m_scrollerPair->updateValues();
#endif
}

bool ScrollingTreeScrollingNodeDelegateCoordinated::handleWheelEvent(const PlatformWheelEvent& wheelEvent)
{
    auto deferrer = ScrollingTreeWheelEventTestMonitorCompletionDeferrer { *scrollingTree(), scrollingNode()->scrollingNodeID(), WheelEventTestMonitor::DeferReason::HandlingWheelEvent };

    updateUserScrollInProgressForEvent(wheelEvent);

#if PLATFORM(CUTEFISH)
    if (handleOverscroll(wheelEvent))
        return true;
#endif

    return m_scrollController.handleWheelEvent(wheelEvent);
}

#if PLATFORM(CUTEFISH)
// A scroller inside the page stretches past its end under a touchpad's
// scroll, less the further it is out, and springs back when the fingers
// lift and the momentum is spent; the page around it stays put.
bool ScrollingTreeScrollingNodeDelegateCoordinated::handleOverscroll(const PlatformWheelEvent& wheelEvent)
{
    Ref node = scrollingNode();
    if (!is<ScrollingTreeOverflowScrollingNode>(node.get()) || !wheelEvent.hasPreciseScrollingDeltas())
        return false;

    const auto minimum = minimumScrollPosition();
    const auto maximum = maximumScrollPosition();
    const auto position = currentScrollPosition();
    const auto clamped = position.constrainedBetween(minimum, maximum);

    const bool ending = wheelEvent.phase() == PlatformWheelEventPhase::Ended
        || wheelEvent.phase() == PlatformWheelEventPhase::Cancelled
        || wheelEvent.momentumPhase() == PlatformWheelEventPhase::Ended
        || wheelEvent.momentumPhase() == PlatformWheelEventPhase::Cancelled;
    if (ending) {
        // The momentum that follows lifted fingers keeps stretching until it
        // ends.
        if (wheelEvent.phase() == PlatformWheelEventPhase::Ended && wheelEvent.momentumPhase() == PlatformWheelEventPhase::None
            && m_overscrolled)
            return true;
        if (!m_overscrolled)
            return false;
        m_overscrolled = false;
        if (!startAnimatedScrollToPosition(clamped))
            node->scrollTo(clamped, ScrollType::User, ScrollClamping::Clamped);
        return true;
    }

    const bool moving = wheelEvent.phase() == PlatformWheelEventPhase::Began || wheelEvent.phase() == PlatformWheelEventPhase::Changed
        || wheelEvent.momentumPhase() == PlatformWheelEventPhase::Began || wheelEvent.momentumPhase() == PlatformWheelEventPhase::Changed;
    if (!moving)
        return false;

    FloatSize change = -wheelEvent.delta();
    if (!allowsHorizontalScrolling())
        change.setWidth(0);
    if (!allowsVerticalScrolling())
        change.setHeight(0);
    const auto visible = scrollableAreaSize();
    auto stretchAxis = [](float at, float low, float high, float delta, float length) {
        float beyond = at < low ? low - at : at > high ? at - high : 0;
        bool outward = (at <= low && delta < 0) || (at >= high && delta > 0);
        if (!beyond && !outward)
            return std::optional<float> { };
        if (!outward) {
            // Back towards the content: the stretch gives way first.
            float next = at + delta;
            return std::optional<float> { at < low ? std::min(next, low) : std::max(next, high) };
        }
        float limit = std::max(length * 0.25f, 1.0f);
        float resistance = std::max(0.0f, 1 - beyond / limit);
        float next = at + delta * resistance * 0.5f;
        return std::optional<float> { at <= low ? std::max(next, low - limit) : std::min(next, high + limit) };
    };
    auto x = stretchAxis(position.x(), minimum.x(), maximum.x(), change.width(), visible.width());
    auto y = stretchAxis(position.y(), minimum.y(), maximum.y(), change.height(), visible.height());
    if (!x && !y)
        return false;

    FloatPoint target(x ? *x : clamped.x(), y ? *y : clamped.y());
    if (!x)
        target.setX(std::clamp(position.x() + change.width(), minimum.x(), maximum.x()));
    if (!y)
        target.setY(std::clamp(position.y() + change.height(), minimum.y(), maximum.y()));
    stopAnimatedScroll({ });
    node->scrollTo(target, ScrollType::User, ScrollClamping::Unclamped);
    m_overscrolled = target != target.constrainedBetween(minimum, maximum);
    return true;
}
#endif

#if USE(COORDINATED_GRAPHICS_ASYNC_SCROLLBAR)
void ScrollingTreeScrollingNodeDelegateCoordinated::updateFromStateNode(const ScrollingStateScrollingNode& scrollingStateNode)
{
    ASSERT(isMainThread());
    CheckedRef horizontalScroller = m_scrollerPair->horizontalScroller();
    CheckedRef verticalScroller = m_scrollerPair->verticalScroller();

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::PainterForScrollbar)) {
        horizontalScroller->setScrollerImp(scrollingStateNode.horizontalScrollerImp());
        verticalScroller->setScrollerImp(scrollingStateNode.verticalScrollerImp());
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::ScrollbarHoverState)) {
        auto hoverState = scrollingStateNode.scrollbarHoverState();
        verticalScroller->setHoveredAndPressedParts(hoverState.hoveredPartInVerticalScrollbar, hoverState.pressedPartInVerticalScrollbar);
        horizontalScroller->setHoveredAndPressedParts(hoverState.hoveredPartInHorizontalScrollbar, hoverState.pressedPartInHorizontalScrollbar);
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::HorizontalScrollbarLayer))
        horizontalScroller->setHostLayer(static_cast<CoordinatedPlatformLayer*>(scrollingStateNode.horizontalScrollbarLayer()));

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::VerticalScrollbarLayer))
        verticalScroller->setHostLayer(static_cast<CoordinatedPlatformLayer*>(scrollingStateNode.verticalScrollbarLayer()));

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::ScrollbarEnabledState)) {
        auto scrollbarEnabledState = scrollingStateNode.scrollbarEnabledState();
        horizontalScroller->setEnabled(scrollbarEnabledState.horizontalScrollbarIsEnabled);
        verticalScroller->setEnabled(scrollbarEnabledState.verticalScrollbarIsEnabled);
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::ScrollbarLayoutDirection)) {
        auto scrollbarLayoutDirection = scrollingStateNode.scrollbarLayoutDirection();
        horizontalScroller->setScrollbarLayoutDirection(scrollbarLayoutDirection);
        verticalScroller->setScrollbarLayoutDirection(scrollbarLayoutDirection);
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::UseDarkAppearanceForScrollbars)) {
        bool useDarkAppearanceForScrollbars = scrollingStateNode.useDarkAppearanceForScrollbars();
        horizontalScroller->setUseDarkAppearance(useDarkAppearanceForScrollbars);
        verticalScroller->setUseDarkAppearance(useDarkAppearanceForScrollbars);
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::OverlayScrollbarsEnabled)) {
        if (auto* scrollingStateFrameScrollingNode = dynamicDowncast<ScrollingStateFrameScrollingNode>(&scrollingStateNode)) {
            auto overlayScrollbarsEnabled = scrollingStateFrameScrollingNode->overlayScrollbarsEnabled();
            horizontalScroller->setOverlayScrollbarEnabled(overlayScrollbarsEnabled);
            verticalScroller->setOverlayScrollbarEnabled(overlayScrollbarsEnabled);
        }
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::ScrollbarOpacity)) {
        float scrollbarOpacity = scrollingStateNode.scrollbarOpacity();
        horizontalScroller->setOpacity(scrollbarOpacity);
        verticalScroller->setOpacity(scrollbarOpacity);
    }

    if (scrollingStateNode.hasChangedProperty(ScrollingStateNode::Property::ScrollbarColor)) {
        horizontalScroller->setScrollbarColor(scrollingStateNode.scrollbarColor());
        verticalScroller->setScrollbarColor(scrollingStateNode.scrollbarColor());
    }

    ThreadedScrollingTreeScrollingNodeDelegate::updateFromStateNode(scrollingStateNode);
}
#endif

} // namespace WebCore

#endif // ENABLE(ASYNC_SCROLLING) && USE(COORDINATED_GRAPHICS)
