#include "playlisteditdialog.hpp"

PlaylistEditDialog::PlaylistEditDialog(const spt::Playlist &playlist, int selectedIndex, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(playlist.name);
	setModal(true);
	auto layout = new QVBoxLayout(this);
	// Name
	layout->addWidget(new QLabel("Name:", this));
	name = new QLineEdit(playlist.name, this);
	layout->addWidget(name);
	// Description
	layout->addWidget(new QLabel("Description:", this));
	description = new QTextEdit(playlist.description, this);
	layout->addWidget(description);
	// Toggles
	auto toggles = new QHBoxLayout();
	toggles->setAlignment(Qt::AlignLeft);
	isPublic = new QCheckBox("Public", this);
	isPublic->setChecked(playlist.isPublic);
	toggles->addWidget(isPublic);
	isCollaborative = new QCheckBox("Collaborative", this);
	isCollaborative->setChecked(playlist.collaborative);
	toggles->addWidget(isCollaborative);
	layout->addLayout(toggles);
	// Focus depending on selected index
	if (selectedIndex < 0)
		setFocus();
	else if (selectedIndex == 0)
		name->setFocus();
	else if (selectedIndex == 1)
		description->setFocus();
	// Dialog buttons
	auto buttons = new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	QDialogButtonBox::connect(buttons, &QDialogButtonBox::accepted, [this] {
		accept();
	});
	QDialogButtonBox::connect(buttons, &QDialogButtonBox::rejected, [this] {
		reject();
	});
	layout->addWidget(buttons);
	setLayout(layout);
}