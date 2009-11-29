/*
 * This file is part of the Ideal Library
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

#include <core/application.h>
#include <core/object.h>

// Note: a, b and c are three parameters just to show that having your own parameters in signals
// is the same in multi slots as in regular slots. Nothing changes.

class MediaButton
    : public IdealCore::Object
{
public:
    MediaButton(IdealCore::Object *parent);

    void emitPressed(iint32 a, iint32 b, iint32 c);

public:
    IDEAL_SIGNAL(buttonPressed, int, int, int);
};

MediaButton::MediaButton(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(buttonPressed, int, int, int)
{
}

void MediaButton::emitPressed(iint32 a, iint32 b, iint32 c)
{
    emit(buttonPressed, a, b, c);
}

class MediaPlayer
    : public IdealCore::Object
{
public:
    MediaPlayer(IdealCore::Object *parent);
    
    void actionReceived(IdealCore::Object *sender, iint32 a, iint32 b, iint32 c);

public:
    MediaButton *play;
    MediaButton *pause;
    MediaButton *stop;
    MediaButton *exit;
};

MediaPlayer::MediaPlayer(Object *parent)
    : Object(parent)
    , play(new MediaButton(this))
    , pause(new MediaButton(this))
    , stop(new MediaButton(this))
    , exit(new MediaButton(this))
{
    connectMulti(play->buttonPressed, this, &MediaPlayer::actionReceived);
    connectMulti(pause->buttonPressed, this, &MediaPlayer::actionReceived);
    connectMulti(stop->buttonPressed, this, &MediaPlayer::actionReceived);
    connectMulti(exit->buttonPressed, this, &MediaPlayer::actionReceived);
}

void MediaPlayer::actionReceived(IdealCore::Object *sender, iint32 a, iint32 b, iint32 c)
{
    IDEAL_DEBUG("(" << a << ", " << b << ", " << c << ")");
    if (sender == play) {
        IDEAL_DEBUG("play button pressed");
    } else if (sender == pause) {
        IDEAL_DEBUG("pause button pressed");
    } else if (sender == stop) {
        IDEAL_DEBUG("stop button pressed");
    } else if (sender == exit) {
        IDEAL_DEBUG("exit button pressed");
    } else {
        IDEAL_DEBUG("unknown button pressed");
    }
}

int main(int argc, char **argv)
{
    IdealCore::Application app(argc, argv);
    MediaPlayer mediaPlayer(&app);

    // user will play, pause, play again, stop and then exit
    mediaPlayer.play->emitPressed(0, 1, 2);
    mediaPlayer.pause->emitPressed(1, 2, 1);
    mediaPlayer.play->emitPressed(5, 1, 8);
    mediaPlayer.stop->emitPressed(6, 4, 2);
    mediaPlayer.exit->emitPressed(9, 3, 1);
    
    return 0;
}
