#pragma once

#include <QLabel>
#include <QDockWidget>

namespace Widget
{
	class DockTitle: public QLabel
	{
	public:
		DockTitle(const QString &title, QDockWidget *parent);

		static auto margins() -> QMargins;

	private:
		static constexpr int marginVertical = 6;
		static constexpr int marginHorizontal = 12;
	};
}
