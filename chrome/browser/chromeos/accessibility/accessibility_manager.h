// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H_
#define CHROME_BROWSER_CHROMEOS_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H_

#include <set>

#include "ash/accessibility_delegate.h"
#include "ash/session_state_observer.h"
#include "base/callback_list.h"
#include "base/memory/weak_ptr.h"
#include "base/prefs/pref_change_registrar.h"
#include "base/time/time.h"
#include "chrome/browser/chromeos/accessibility/accessibility_util.h"
#include "chrome/browser/extensions/api/braille_display_private/braille_controller.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_system.h"

namespace content {
class RenderViewHost;
}
class Profile;

namespace chromeos {

enum AccessibilityNotificationType {
  ACCESSIBILITY_MANAGER_SHUTDOWN,
  ACCESSIBILITY_TOGGLE_HIGH_CONTRAST_MODE,
  ACCESSIBILITY_TOGGLE_LARGE_CURSOR,
  ACCESSIBILITY_TOGGLE_SCREEN_MAGNIFIER,
  ACCESSIBILITY_TOGGLE_SPOKEN_FEEDBACK,
  ACCESSIBILITY_TOGGLE_VIRTUAL_KEYBOARD
};

struct AccessibilityStatusEventDetails {
  AccessibilityStatusEventDetails(
      AccessibilityNotificationType notification_type,
      bool enabled,
      ash::AccessibilityNotificationVisibility notify);

  AccessibilityStatusEventDetails(
      AccessibilityNotificationType notification_type,
      bool enabled,
      ash::MagnifierType magnifier_type,
      ash::AccessibilityNotificationVisibility notify);

  AccessibilityNotificationType notification_type;
  bool enabled;
  ash::MagnifierType magnifier_type;
  ash::AccessibilityNotificationVisibility notify;
};

typedef base::Callback<void(const AccessibilityStatusEventDetails&)>
    AccessibilityStatusCallback;

typedef base::CallbackList<void(const AccessibilityStatusEventDetails&)>
    AccessibilityStatusCallbackList;

typedef AccessibilityStatusCallbackList::Subscription
    AccessibilityStatusSubscription;

// AccessibilityManager changes the statuses of accessibility features
// watching profile notifications and pref-changes.
// TODO(yoshiki): merge MagnificationManager with AccessibilityManager.
class AccessibilityManager : public content::NotificationObserver,
    extensions::api::braille_display_private::BrailleObserver,
    public ash::SessionStateObserver {
 public:
  // Creates an instance of AccessibilityManager, this should be called once,
  // because only one instance should exist at the same time.
  static void Initialize();
  // Deletes the existing instance of AccessibilityManager.
  static void Shutdown();
  // Returns the existing instance. If there is no instance, returns NULL.
  static AccessibilityManager* Get();

  // On a user's first login into a device, any a11y features enabled/disabled
  // by the user on the login screen are enabled/disabled in the user's profile.
  // This class watches for profile changes and copies settings into the user's
  // profile when it detects a login with a newly created profile.
  class PrefHandler {
   public:
    explicit PrefHandler(const char* pref_path);
    virtual ~PrefHandler();

    // Should be called from AccessibilityManager::SetProfile().
    void HandleProfileChanged(Profile* previous_profile,
                              Profile* current_profile);

   private:
    const char* pref_path_;
  };

  // Returns true when the accessibility menu should be shown.
  bool ShouldShowAccessibilityMenu();

  // Returns true when cursor compositing should be enabled.
  bool ShouldEnableCursorCompositing();

  // Enables or disables the large cursor.
  void EnableLargeCursor(bool enabled);
  // Returns true if the large cursor is enabled, or false if not.
  bool IsLargeCursorEnabled();

  // Enables or disable Sticky Keys.
  void EnableStickyKeys(bool enabled);

  // Returns true if Incognito mode is allowed, or false if not.
  bool IsIncognitoAllowed();

  // Returns true if the Sticky Keys is enabled, or false if not.
  bool IsStickyKeysEnabled();

  // Enables or disables spoken feedback. Enabling spoken feedback installs the
  // ChromeVox component extension.
  void EnableSpokenFeedback(bool enabled,
                            ash::AccessibilityNotificationVisibility notify);

  // Returns true if spoken feedback is enabled, or false if not.
  bool IsSpokenFeedbackEnabled();

  // Toggles whether Chrome OS spoken feedback is on or off.
  void ToggleSpokenFeedback(ash::AccessibilityNotificationVisibility notify);

  // Enables or disables the high contrast mode for Chrome.
  void EnableHighContrast(bool enabled);

  // Returns true if High Contrast is enabled, or false if not.
  bool IsHighContrastEnabled();

  // Enables or disables autoclick.
  void EnableAutoclick(bool enabled);

  // Returns true if autoclick is enabled.
  bool IsAutoclickEnabled();

  // Set the delay for autoclicking after stopping the cursor in milliseconds.
  void SetAutoclickDelay(int delay_ms);

  // Returns the autoclick delay in milliseconds.
  int GetAutoclickDelay() const;

  // Enables or disables the virtual keyboard.
  void EnableVirtualKeyboard(bool enabled);
  // Returns true if the virtual keyboard is enabled, otherwise false.
  bool IsVirtualKeyboardEnabled();

  // SessionStateObserver overrides:
  virtual void ActiveUserChanged(const std::string& user_id) OVERRIDE;

  void SetProfileForTest(Profile* profile);

  static void SetBrailleControllerForTest(
      extensions::api::braille_display_private::BrailleController* controller);

  // Enables/disables system sounds.
  void EnableSystemSounds(bool system_sounds_enabled);

  // Initiates play of shutdown sound and returns it's duration.
  base::TimeDelta PlayShutdownSound();

  // Injects ChromeVox scripts into given |render_view_host|.
  void InjectChromeVox(content::RenderViewHost* render_view_host);

  // Register a callback to be notified when the status of an accessibility
  // option changes.
  scoped_ptr<AccessibilityStatusSubscription> RegisterCallback(
      const AccessibilityStatusCallback& cb);

  // Notify registered callbacks of a status change in an accessibility setting.
  void NotifyAccessibilityStatusChanged(
      AccessibilityStatusEventDetails& details);

  // Notify accessibility when locale changes occur.
  void OnLocaleChanged();

 protected:
  AccessibilityManager();
  virtual ~AccessibilityManager();

 private:
  void LoadChromeVox();
  void LoadChromeVoxToUserScreen();
  void LoadChromeVoxToLockScreen();
  void UnloadChromeVox();
  void UnloadChromeVoxFromLockScreen();
  void PostLoadChromeVox(Profile* profile);
  void PostUnloadChromeVox(Profile* profile);

  void UpdateLargeCursorFromPref();
  void UpdateStickyKeysFromPref();
  void UpdateSpokenFeedbackFromPref();
  void UpdateHighContrastFromPref();
  void UpdateAutoclickFromPref();
  void UpdateAutoclickDelayFromPref();
  void UpdateVirtualKeyboardFromPref();

  void CheckBrailleState();
  void ReceiveBrailleDisplayState(
      scoped_ptr<extensions::api::braille_display_private::DisplayState> state);

  void SetProfile(Profile* profile);

  void UpdateChromeOSAccessibilityHistograms();

  // content::NotificationObserver implementation:
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // extensions::api::braille_display_private::BrailleObserver implementation.
  // Enables spoken feedback if a braille display becomes available.
  virtual void OnDisplayStateChanged(
      const extensions::api::braille_display_private::DisplayState&
          display_state) OVERRIDE;

  // Profile which has the current a11y context.
  Profile* profile_;

  // Profile which ChromeVox is currently loaded to. If NULL, ChromeVox is not
  // loaded to any profile.
  bool chrome_vox_loaded_on_lock_screen_;
  bool chrome_vox_loaded_on_user_screen_;

  content::NotificationRegistrar notification_registrar_;
  scoped_ptr<PrefChangeRegistrar> pref_change_registrar_;
  scoped_ptr<PrefChangeRegistrar> local_state_pref_change_registrar_;
  scoped_ptr<ash::ScopedSessionStateObserver> session_state_observer_;

  PrefHandler large_cursor_pref_handler_;
  PrefHandler spoken_feedback_pref_handler_;
  PrefHandler high_contrast_pref_handler_;
  PrefHandler autoclick_pref_handler_;
  PrefHandler autoclick_delay_pref_handler_;
  PrefHandler virtual_keyboard_pref_handler_;

  bool large_cursor_enabled_;
  bool sticky_keys_enabled_;
  bool spoken_feedback_enabled_;
  bool high_contrast_enabled_;
  bool autoclick_enabled_;
  int autoclick_delay_ms_;
  bool virtual_keyboard_enabled_;

  ash::AccessibilityNotificationVisibility spoken_feedback_notification_;

  base::WeakPtrFactory<AccessibilityManager> weak_ptr_factory_;

  bool should_speak_chrome_vox_announcements_on_user_screen_;

  bool system_sounds_enabled_;

  AccessibilityStatusCallbackList callback_list_;

  DISALLOW_COPY_AND_ASSIGN(AccessibilityManager);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_ACCESSIBILITY_ACCESSIBILITY_MANAGER_H_
