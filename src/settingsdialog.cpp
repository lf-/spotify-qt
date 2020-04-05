#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QGridLayout();
	// Add app settings to main layout
	mainLayout->addWidget(appSettings(), 0, 0, Qt::AlignTop);
	// Add spotifyd settings to main layout
	mainLayout->addWidget(spotifySettings(), 0, 1);
	// Buttons
	auto okButton = new QPushButton("OK");
	QPushButton::connect(okButton, &QPushButton::clicked, [=](bool checked) {
		if (applySettings())
			accept();
	});
	auto applyButton = new QPushButton("Apply");
	QPushButton::connect(applyButton, &QPushButton::clicked, [=](bool checked) {
		applySettings();
	});
	auto cancelButton = new QPushButton("Cancel");
	QPushButton::connect(cancelButton, &QPushButton::clicked, [=](bool checked) {
		accept();
	});
	auto buttons = new QHBoxLayout();
	buttons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttons->addWidget(okButton);
	buttons->addWidget(applyButton);
	buttons->addWidget(cancelButton);
	mainLayout->addLayout(buttons, 1, 0, 1, 2);
	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
}

QGroupBox *SettingsDialog::appSettings()
{
	auto appSettings = new QGroupBox("Application", this);
	auto appLayout = new QGridLayout(this);
	appSettings->setLayout(appLayout);
	// Theme
	auto appThemeLabel = new QLabel("Theme", this);
	appThemeLabel->setToolTip("Theme used throughout the application");
	appLayout->addWidget(appThemeLabel, 0, 0);
	appTheme = new QComboBox(this);
	appTheme->addItems(QStyleFactory::keys());
	for (auto &style : QStyleFactory::keys())
		if (style.toLower() == QApplication::style()->objectName())
			appTheme->setCurrentText(style);
	appLayout->addWidget(appTheme, 0, 1);
	// Style palette
	auto appPaletteLabel = new QLabel("Colors", this);
	appPaletteLabel->setToolTip("What color palette to use with current theme");
	appLayout->addWidget(appPaletteLabel, 1, 0);
	appPalette = new QComboBox(this);
	appPalette->addItems({
		"Default", "From style", "Dark"
	});
	appPalette->setCurrentIndex(settings.stylePalette());
	appLayout->addWidget(appPalette, 1, 1);
	// Refresh interval
	auto appRefreshLabel = new QLabel("Refresh interval", this);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	appLayout->addWidget(appRefreshLabel);
	auto appRefresh = new QComboBox(this);
	appRefresh->addItems({
		"Fast (3s)", "Medium (5s)", "Slow (10s)"
	});
	appRefresh->setCurrentIndex(1);
	appLayout->addWidget(appRefresh);
	// Start client
	sptStartClient = new QCheckBox("Autostart spotifyd", this);
	sptStartClient->setToolTip("Start spotifyd together with the app");
	sptStartClient->setChecked(settings.sptStartClient());
	appLayout->addWidget(sptStartClient, 3, 0, 1, 2);
	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", this);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.pulseVolume());
		appLayout->addWidget(appPulse, 4, 0, 1, 2);
	}
	// MPRIS D-Bus
	appMedia = new QCheckBox("Media Controller", this);
	appMedia->setChecked(settings.mediaController());
#ifdef Q_OS_LINUX
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface, buggy (issue #4)");
#else
	appMedia->setToolTip("Currently only available on Linux");
	appMedia->setEnabled(false);
#endif
	appLayout->addWidget(appMedia, 5, 0, 1, 2);

	return appSettings;
}

QGroupBox *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto sptSettings = new QGroupBox("spotifyd", this);
	auto sptMainLayout = new QVBoxLayout(this);
	sptSettings->setLayout(sptMainLayout);
	// Executable settings
	sptPath = new QLineEdit(settings.sptPath(), this);
	sptPath->setPlaceholderText("spotifyd path");
	sptMainLayout->addWidget(sptPath);
	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", this);
	if (!settings.sptPath().isEmpty())
		sptVersion->setText(sptClient(settings.sptPath()));
	sptVersion->setEnabled(false);
	sptMainLayout->addWidget(sptVersion);
	// Layout for all settings
	auto sptLayout = new QGridLayout();
	sptLayout->setEnabled(false);
	sptMainLayout->addLayout(sptLayout);
	// Username
	sptLayout->addWidget(new QLabel("Username", this), 0, 0);
	sptUsername = new QLineEdit(settings.sptUser(), this);
	sptLayout->addWidget(sptUsername, 0, 1);
	// Bitrate
	sptLayout->addWidget(new QLabel("Quality", this));
	sptBitrate = new QComboBox(this);
	sptBitrate->addItems({
		"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
	});
	auto bitrate = settings.sptBitrate();
	sptBitrate->setCurrentIndex(bitrate == 96 ? 0 : bitrate == 160 ? 1 : 2);
	sptLayout->addWidget(sptBitrate);
	// Final layout
	return sptSettings;
}

QString SettingsDialog::sptClient(const QString &path)
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
		return QString();
	// Check if either client
	if (file.baseName() != "spotifyd")
		return QString();
	// Prepare process
	QProcess process;
	// Get version info
	process.start(file.absoluteFilePath(), {
		"--version"
	});
	process.waitForFinished();
	// Entire stdout is version
	return process.readAllStandardOutput();
}

bool SettingsDialog::applySettings()
{
	// Set theme
	QApplication::setStyle(appTheme->currentText());
	settings.setStyle(appTheme->currentText());

	// Warn when changing palette
	auto palette = (Settings::Palette) appPalette->currentIndex();
	if (palette != settings.stylePalette())
		QMessageBox::information(
			this, "Palette",
			"Please restart the application to fully apply selected palette");

	// Set palette
	MainWindow::applyPalette(palette);
	settings.setStylePalette(palette);

	// Media controller
	if (appMedia->isChecked() != settings.mediaController())
		QMessageBox::information(
			this, "Media Controller",
			"Please restart the application to apply changes");
	settings.setMediaController(appMedia->isChecked());

	// PulseAudio volume
	if (appPulse != nullptr)
		settings.setPulseVolume(appPulse->isChecked());

	// Check spotify client path
	if (!sptPath->text().isEmpty())
	{
		auto client = sptClient(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotifyd path");
			return false;
		}
		sptVersion->setText(client);
		settings.setSptPath(sptPath->text());
	}

	// Other Spotify stuff
	settings.setSptStartClient(sptStartClient->isChecked());
	settings.setSptUser(sptUsername->text());
	auto bitrate = sptBitrate->currentIndex();
	settings.setSptBitrate(bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320);

	// Everything is fine
	return true;
}

void SettingsDialog::applyFail(const QString &setting)
{
	QMessageBox::warning(this,
		"Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(setting));
}

bool SettingsDialog::isPulse()
{
	// Assume /usr/bin/pactl
	return QFileInfo("/usr/bin/pactl").isExecutable();
}
