/*
 * This file is part of the Ideal libraries
 * Copyright (C) 2009 Rafael Fernández López <ereslibre@ereslibre.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <core/object.h>
#include <core/application.h>

class MediaButton
    : public IdealCore::Object
{
public:
    MediaButton(Object *parent);

    IDEAL_SIGNAL(clicked);
};

MediaButton::MediaButton(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(clicked)
{
}

class MediaPlayer
    : public IdealCore::Object
{
public:
    MediaPlayer(Object *parent);

    void simulatePlayPauseClick();
    void simulateStopClick();
    void simulateQuitClick();

    void executeAction(Object *sender);

private:
    MediaButton *m_playPause;
    MediaButton *m_stop;
    MediaButton *m_quit;
};

MediaPlayer::MediaPlayer(Object *parent)
    : Object(parent)
    , m_playPause(new MediaButton(this))
    , m_stop(new MediaButton(this))
    , m_quit(new MediaButton(this))
{
    connectMulti(m_playPause->clicked, this, &MediaPlayer::executeAction);
    connectMulti(m_stop->clicked, this, &MediaPlayer::executeAction);
    connectMulti(m_quit->clicked, this, &MediaPlayer::executeAction);
}

void MediaPlayer::simulatePlayPauseClick()
{
    emit(m_playPause->clicked);
}

void MediaPlayer::simulateStopClick()
{
    emit(m_stop->clicked);
}

void MediaPlayer::simulateQuitClick()
{
    emit(m_quit->clicked);
}

void MediaPlayer::executeAction(Object *sender)
{
    if (sender == m_playPause) {
        IDEAL_SDEBUG("Play or pause was clicked");
    } else if (sender == m_stop ) {
        IDEAL_SDEBUG("Stop was clicked");
    } else {
        IDEAL_SDEBUG("Quit was clicked");
        application()->quit();
    }
}

int main(int argc, char **argv)
{
    IdealCore::Application app(argc, argv);

    MediaPlayer *mediaPlayer = new MediaPlayer(&app);
    mediaPlayer->simulatePlayPauseClick();
    mediaPlayer->simulateStopClick();
    mediaPlayer->simulateQuitClick();

    return 0;
}

