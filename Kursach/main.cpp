#include <SFML/Graphics.hpp>

using namespace sf;

const int H = 9;
const int W = 15;

String Level1[H] = {
	"WWWWWWWWWWWWWWW",
	"W             W",
	"W             W",
	"W             W",
	"W             W",
	"W             W",
	"W             W",
	"W             W",
	"WWWWWWWWWWWWWWW",
};

class PLAYER {
public:
	float dx, dy;		//координати в даний момент часу
	FloatRect rect;		//Збереження інформації про спрайт
	Sprite sprite;		//Сам спрайт поточного зображення
	int life;			//кількість життів
	float currentFrame;	//поточний кадр анімації

	//Конструкторр класа
	PLAYER(Texture &image)
	{
		sprite.setTexture(image);
		rect = FloatRect(0, 0, 32, 48);
		dx = dy = 0.1;
		currentFrame = 0;
	}

	void update(float time)
	{	
		rect.left += dx * time;
		rect.top += dy*time;
		currentFrame += 0.0005*time;
		if (currentFrame > 3) currentFrame -= 3;
		//подгрузка анимации, координати задаються Верхній лівий кут + ширина, висота персонажа.
		if (dx>0) sprite.setTextureRect(IntRect(96 * int(currentFrame), 128, 96, 64));
		if (dx<0) sprite.setTextureRect(IntRect(96 * int(currentFrame), 64, 96, 64));
		if (dy>0) sprite.setTextureRect(IntRect(96 * int(currentFrame), 0, 96, 64));
		if (dy<0) sprite.setTextureRect(IntRect(96 * int(currentFrame), 192, 96, 64));
		sprite.setPosition(rect.left, rect.top);
		dx = 0;
		dy = 0;
	}
};


int main()
{
	RenderWindow window(VideoMode(960, 576), "Kursach game!");
	Clock clock;
	Texture t;
	t.loadFromFile("player.png");
	
	float currentFrame = 0;

	PLAYER p(t);


	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 25;
		if (time > 20) time = 20;
		Event event;
					while (window.pollEvent(event))
					{
						if (event.type == sf::Event::Closed)
						window.close();

						if (Keyboard::isKeyPressed(Keyboard::Left))
							p.dx = -0.1;
						if (Keyboard::isKeyPressed(Keyboard::Right))
							p.dx = 0.1;
						if (Keyboard::isKeyPressed(Keyboard::Up))
							p.dy = -0.1;
						if (Keyboard::isKeyPressed(Keyboard::Down))
							p.dy = 0.1;
					}
					p.update(time);
				window.clear(Color::White);
			window.draw(p.sprite);
		window.display();
	}

	return 0;
}