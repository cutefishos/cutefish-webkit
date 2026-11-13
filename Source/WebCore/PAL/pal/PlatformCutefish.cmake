list(APPEND PAL_PUBLIC_HEADERS
    system/glib/SleepDisablerGLib.h
)

list(APPEND PAL_SOURCES
    crypto/openssl/CryptoDigestOpenSSL.cpp

    system/ClockGeneric.cpp
    system/Sound.cpp

    system/glib/SleepDisablerGLib.cpp

    text/KillRing.cpp
)

list(APPEND PAL_LIBRARIES
    GLib::Gio
    OpenSSL::Crypto
)
