// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKSpellChecker_h
#define WKSpellChecker_h

#include <WebKit/WKBase.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The embedder's spell checker, which every page asks. Text is UTF-16, and
// offsets and lengths count its code units.
typedef void (*WKSpellCheckerAddGuessFunction)(const uint16_t* guess, size_t length, void* addContext);

typedef struct WKSpellCheckerClient {
    void* context;
    // The first misspelled word of the text: its offset, and a length of 0
    // when there is none.
    void (*checkSpelling)(const uint16_t* text, size_t length, size_t* location, size_t* misspelledLength, void* context);
    // Calls add for each replacement of a misspelled word, best first.
    void (*guesses)(const uint16_t* word, size_t length, WKSpellCheckerAddGuessFunction add, void* addContext, void* context);
    void (*learnWord)(const uint16_t* word, size_t length, void* context);
    void (*ignoreWord)(const uint16_t* word, size_t length, void* context);
} WKSpellCheckerClient;

// Pages check spelling only once a client is set; null takes it away.
WK_EXPORT void WKSpellCheckerSetClient(const WKSpellCheckerClient*);
// Whether misspelled words are marked as one types, for every page.
WK_EXPORT void WKSpellCheckerSetContinuousSpellCheckingEnabled(bool);
WK_EXPORT bool WKSpellCheckerGetContinuousSpellCheckingEnabled(void);

#ifdef __cplusplus
}
#endif

#endif // WKSpellChecker_h
