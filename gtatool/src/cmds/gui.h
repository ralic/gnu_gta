/*
 * gui.h
 *
 * This file is part of gtatool, a tool to manipulate Generic Tagged Arrays
 * (GTAs).
 *
 * Copyright (C) 2010  Martin Lambers <marlam@marlam.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_H
#define GUI_H

#include "config.h"

#include <string>
#include <vector>

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QTabWidget>
#include <QDir>

#include <gta/gta.hpp>

#include "cio.h"        // For a fixed off_t on W32


class MyTabWidget : public QTabWidget
{
    /* The only purpose of this class is to get access to tabBar(),
     * which is protected in QTabWidget */
public:
    MyTabWidget(QWidget *parent = NULL) : QTabWidget(parent) {}
    ~MyTabWidget() {}
    QTabBar *tabBar() const { return QTabWidget::tabBar(); }
};

class TaglistWidget : public QWidget
{
Q_OBJECT

public:
    enum type { global, dimension, component };

private:
    gta::header *_header;
    enum type _type;
    uintmax_t _index;
    bool _cell_change_lock;
    bool _cell_change_add_mode;
    QTableWidget *_tablewidget;
    QPushButton *_remove_button;
    QPushButton *_add_button;

private slots:
    void cell_changed(int row, int column);
    void selection_changed();
    void remove();
    void add();

public:
    TaglistWidget(gta::header *header, enum type type, uintmax_t index = 0, QWidget *parent = NULL);
    ~TaglistWidget();

public slots:
    void update();

signals:
    void changed(gta::header *header, int type, uintmax_t index);
};

class ArrayWidget : public QWidget
{
Q_OBJECT

private:
    gta::header *_header;
    QLabel *_dimensions_label;
    QLabel *_components_label;
    QLabel *_size_label;
    MyTabWidget *_taglists_widget;

private slots:
    void taglist_changed(gta::header *header, int type, uintmax_t index);

public:
    ArrayWidget(gta::header *_header, QWidget *parent = NULL);
    ~ArrayWidget();

    void saved();

public slots:
    void update();

signals:
    void changed(gta::header *header);
};

class FileWidget : public QWidget
{
Q_OBJECT

private:
    FILE *_f;
    std::string _name;
    std::string _temp_name;
    bool _is_changed;
    std::vector<gta::header *> _headers;
    std::vector<off_t> _offsets;
    MyTabWidget *_arrays_widget;

private slots:
    void array_changed(gta::header *header);

public:

    FileWidget(FILE *f, const std::string &name, const std::string &temp_name,
            const std::vector<gta::header *> &headers,
            const std::vector<off_t> &offsets,
            QWidget *parent = NULL);
    ~FileWidget();

    FILE *file() const
    {
        return _f;
    }

    const std::string &name() const
    {
        return _name;
    }

    const std::string &temp_name() const
    {
        return _temp_name;
    }

    const std::vector<gta::header *> &headers() const
    {
        return _headers;
    }

    const std::vector<off_t> &offsets() const
    {
        return _offsets;
    }

    bool is_changed() const
    {
        return _is_changed;
    }

    void saved(FILE *f);

    void set_name(const std::string &name);

signals:
    void changed(const std::string &name, const std::string &temp_name);
};

class GUI : public QMainWindow
{
Q_OBJECT

private:
    MyTabWidget *_files_widget;
    QDir _last_file_open_dir;
    QDir _last_file_save_as_dir;

private slots:
    void file_changed(const std::string &name, const std::string &temp_name);

protected:
    void closeEvent(QCloseEvent *event);	
	
public:
    GUI();
    ~GUI();

    void open(const std::string &filename);

private slots:
    void file_open();
    void file_save();
    void file_save_as();
    void file_save_all();
    void file_close();
    void file_close_all();
    void help_about();
};

#endif