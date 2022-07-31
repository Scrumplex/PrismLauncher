#pragma once

#include "ExternalUpdater.h"

#include <QTimer>

class DummyUpdater : public ExternalUpdater
{
	Q_OBJECT
public:
	/*!
	 * Start the Dummy updater, which automatically checks for updates if necessary.
	 */
	DummyUpdater();

	/*!
	 * Check for updates manually, showing the user a progress bar and an alert if no updates are found.
	 */
	void checkForUpdates() override;

	/*!
	 * Indicates whether or not to check for updates automatically.
	 */
	bool getAutomaticallyChecksForUpdates() override;

	/*!
	 * Indicates the current automatic update check interval in seconds.
	 */
	double getUpdateCheckInterval() override;

	/*!
	 * Indicates whether or not beta updates should be checked for in addition to regular releases.
	 */
	bool getBetaAllowed() override;

	/*!
	 * Set whether or not to check for updates automatically.
	 *
	 * As per Sparkle documentation, "By default, Sparkle asks users on second launch for permission if they want
	 * automatic update checks enabled and sets this property based on their response. If SUEnableAutomaticChecks is
	 * set in the Info.plist, this permission request is not performed however.
	 *
	 * Setting this property will persist in the host bundle’s user defaults. Only set this property if you need
	 * dynamic behavior (e.g. user preferences).
	 *
	 * The update schedule cycle will be reset in a short delay after the property’s new value is set. This is to allow
	 * reverting this property without kicking off a schedule change immediately."
	 */
	void setAutomaticallyChecksForUpdates(bool check) override;

	/*!
	 * Set the current automatic update check interval in seconds.
	 *
	 * As per Sparkle documentation, "Setting this property will persist in the host bundle’s user defaults. For this
	 * reason, only set this property if you need dynamic behavior (eg user preferences). Otherwise prefer to set
	 * SUScheduledCheckInterval directly in your Info.plist.
	 *
	 * The update schedule cycle will be reset in a short delay after the property’s new value is set. This is to allow
	 * reverting this property without kicking off a schedule change immediately."
	 */
	void setUpdateCheckInterval(double seconds) override;

	/*!
	 * Set whether or not beta updates should be checked for in addition to regular releases.
	 */
	void setBetaAllowed(bool allowed) override;

public slots:
	void triggerUpdate();

private:
	bool m_updateAvailable;
	bool m_autoUpdate;
	double m_checkInterval;
	bool m_betaAllowed;

	QTimer m_timer;
};
