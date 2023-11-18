#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;


void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}


int main()
{

    RenderWindow window(VideoMode(800, 600), "Balloon Buster");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    Texture redTex;
    LoadTex(redTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> balloons;

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    long redTime(0);

    for (int i(0); i < 6; i++) {
        PhysicsSprite& balloon = balloons.Create();
        balloon.setTexture(redTex);
        int x = 50 + ((700 / 5) * i);
        Vector2f sz = balloon.getSize();
        balloon.setCenter(Vector2f(x, 20 + (sz.y / 2)));
        balloon.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(balloon);
        balloon.onCollision =
            [&drawingArrow, &world, &arrow, &balloon, &balloons, &score]
            (PhysicsBodyCollisionResult result) {
            if (result.object2 == arrow) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(balloon);
                balloons.QueueRemove(balloon);
                score += 10;

            }
            };
    }


    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            redTime = redTime + deltaMS; 
            world.UpdatePhysics(deltaMS);
            MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }
            top.onCollision = [&drawingArrow, &world, &arrow]
            (PhysicsBodyCollisionResult result) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                };
            if (redTime > 700) {
                redTime = 0;
                PhysicsSprite& duck = balloons.Create();
                duck.setTexture(redTex);
                Vector2f ducksz = duck.getSize();
                duck.setCenter(Vector2f(30, 90));
                duck.setVelocity(Vector2f(0.25, 0));
                world.AddPhysicsBody(duck);
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            balloons.DoRemovals();
            for (PhysicsShape& balloon : balloons) {
                window.draw((PhysicsSprite&)balloon);
            }

            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));

            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));

            arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 0));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);

            window.display();

        }
    }
    window.display(); // this is needed to see the last frame
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

