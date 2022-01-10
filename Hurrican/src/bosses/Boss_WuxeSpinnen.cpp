// --------------------------------------------------------------------------------------
// Der Wuxe Spinnenansturm im SpinnenLevel
//
// Spinnen krabbeln von der Decke
// --------------------------------------------------------------------------------------

#include "Boss_WuxeSpinnen.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWuxeSpinnen::GegnerWuxeSpinnen(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_VERFOLGEN;
    BlickRichtung = LINKS;
    Energy = 2000;
    ChangeLight = Light;
    Destroyable = false;
    Value1 = Wert1;
    Value2 = Wert2;
    ActionCounter = 1.0f;
    DontMove = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWuxeSpinnen::DoKI() {
    // Energie anzeigen
    if (Handlung != GEGNER_INIT && Handlung != GEGNER_VERFOLGEN && Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(2000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active == true && Handlung != GEGNER_VERFOLGEN && TileEngine.Zustand == TileStateEnum::SCROLLBAR) {
        TileEngine.ScrollLevel(static_cast<float>(Value1), static_cast<float>(Value2),
                               TileStateEnum::SCROLLTOLOCK);  // Level auf den Boss zentrieren

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
        Gegner.PushGegner(xPos, yPos - 40.0f, EXTRAS, 10, 0, false);
    }

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN) {
        Handlung = GEGNER_EXPLODIEREN;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_INIT:  // Warten bis der Screen zentriert wurde
        {
            if (TileEngine.Zustand == TileStateEnum::LOCKED) {
                // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
                //
                // DKS - Added function SongIsPlaying() to SoundManagerClass:
                if (!SoundManager.SongIsPlaying(MUSIC_BOSS)) {
                    SoundManager.PlaySong(MUSIC_BOSS, false);

                    // Und Boss erscheinen lassen
                    //
                    Handlung = GEGNER_STEHEN;
                }
            }
        } break;

        case GEGNER_STEHEN: {
            // ActionCounter runterzählen
            // bei null Gegner spawnen
            ActionCounter -= 0.8f SYNC;

            if (ActionCounter <= 0.0f) {
                ActionCounter = 1.5f + Energy / 500.0f;

                // und spawnen
                //
                if (Energy > 500.0f) {
                    Gegner.PushGegner(static_cast<float>(Value1 + random(520)),
                                      static_cast<float>(Value2 - 100), MITTELSPINNE, 0, 1, false);
                }

                Energy -= 20.0f;

                // ab und zu ein Paar Augen einstreuen
                //
                if (static_cast<int>(Energy) % 1000 == 0) {
                    for (int i = 0; i < 2; i++) {
                        Gegner.PushGegner(static_cast<float>(Value1 - 20),
                                          static_cast<float>(Value2 + i * 60 + 150), AUGE, random(10), 0,
                                          false);
                        Gegner.PushGegner(static_cast<float>(Value1 + 620),
                                          static_cast<float>(Value2 + i * 60 + 180), AUGE, random(10), 0,
                                          false);
                    }
                }

                // oder AutoFire ;)
                //
                if (static_cast<int>(Energy + 500.0f) % 600 == 0) {
                    Gegner.PushGegner(xPos, yPos - 40.0f, EXTRAS, 10, 0, false);
                }
            }
        } break;

        // warten, bis der Spieler den Gegner berührt hat, und dann aktivieren
        //
        case GEGNER_VERFOLGEN: {
            if (PlayerAbstand() < 400)
                Handlung = GEGNER_INIT;

        } break;

        case GEGNER_EXPLODIEREN: {
            Energy = 0.0f;
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// WuxeSpinnen explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerWuxeSpinnen::GegnerExplode() {
    // Extra Leben
    Gegner.PushGegner(xPos, yPos - 250.0f, ONEUP, 10, 0, false);

    ScrolltoPlayeAfterBoss();
}
