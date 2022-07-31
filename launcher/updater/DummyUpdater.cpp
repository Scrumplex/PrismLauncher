#include "DummyUpdater.h"
#include <QMessageBox>
#include <cstdlib>
#include <QDebug>

DummyUpdater::DummyUpdater()
{
	connect(&m_timer, &QTimer::timeout, this, &DummyUpdater::triggerUpdate);
}

void DummyUpdater::checkForUpdates()
{
	if (m_timer.isActive())
		return;

	if (std::rand() % 2 == 0) {
		qDebug() << "Triggering update in 3s";
		m_timer.setInterval(3000);
		m_timer.setSingleShot(true);
		m_timer.start();
	}
}

bool DummyUpdater::getAutomaticallyChecksForUpdates()
{
	return m_autoUpdate;
}

double DummyUpdater::getUpdateCheckInterval()
{
	return m_checkInterval;
}

bool DummyUpdater::getBetaAllowed()
{
	return m_betaAllowed;
}

void DummyUpdater::setAutomaticallyChecksForUpdates(bool check)
{
	m_autoUpdate = check;
}

void DummyUpdater::setUpdateCheckInterval(double seconds)
{
	m_checkInterval = seconds;
}

void DummyUpdater::setBetaAllowed(bool allowed)
{
	m_betaAllowed = allowed;
}

void DummyUpdater::triggerUpdate()
{
	QMessageBox::information(0, "lol", "lol");
}
