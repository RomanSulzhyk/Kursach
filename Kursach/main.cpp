#include <SFML/Graphics.hpp>

using namespace sf;

const int H = 9;
const int W = 15;

String Level[H];

String Level1[H] = {
	"ZZZZZZZZZZZZZZZ",
	"Z             Z",
	"Z        eee  Z",
	"Z   e         Z",
	"Z    e        o",
	"Z     eee     Z",
	"Z             Z",
	"Z             Z",
	"ZZZZZZZZZZZZZZZ",
};


class PLAYER {
public:
	float dx, dy;		//координати зміщення
	FloatRect rect;		//Збереження інформації про спрайт
	Sprite sprite;		//Сам персонаж поточного зображення
	int life;			//кількість життів
	float currentFrame;	//поточний кадр анімації

	//Конструкторр класа
	PLAYER(Texture &image)
	{
		sprite.setTexture(image);
		rect = FloatRect(0, 0, 48, 48);
		dx = dy = 0.1;
		currentFrame = 0;
		rect.left = 70;
		rect.top = 270;
	}
	void update(float time)
	{	
		rect.left += dx * time;
		rect.top += dy*time;
		currentFrame += 0.0005*time;
		if (currentFrame > 3) currentFrame -= 3;
		//подгрузка анимации, координати задаються Верхній лівий кут + ширина, висота персонажа.
		if (dx>0) sprite.setTextureRect(IntRect(48 * int(currentFrame), 96, 48, 48));
		if (dx<0) sprite.setTextureRect(IntRect(48 * int(currentFrame), 48, 48, 48));
		if (dy>0) sprite.setTextureRect(IntRect(48 * int(currentFrame), 0, 48, 48));
		if (dy<0) sprite.setTextureRect(IntRect(48 * int(currentFrame), 144, 48, 48));
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
	RectangleShape rectangle(Vector2f(64, 64));
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
						//движение по сторонам.
						if (Keyboard::isKeyPressed(Keyboard::A))
							p.dx = -0.1;
						if (Keyboard::isKeyPressed(Keyboard::D))
							p.dx = 0.1;
						if (Keyboard::isKeyPressed(Keyboard::W))
							p.dy = -0.1;
						if (Keyboard::isKeyPressed(Keyboard::S))
							p.dy = 0.1;
					}
					p.update(time);
				window.clear(Color::White);
			
				
				for (int i = 0; i<H; i++)
					for (int j = 0; j<W; j++)
					{
						if (Level1[i][j] == 'Z') rectangle.setFillColor(Color::Black);

						if (Level1[i][j] == 'e')  rectangle.setFillColor(Color::Green);

						if (Level1[i][j] == 'o')  rectangle.setFillColor(Color::Blue);

						if (Level1[i][j] == ' ') continue;

						rectangle.setPosition(j * 64, i * 64);
						window.draw(rectangle);
					}
			window.draw(p.sprite);
		window.display();
	}

	return 0;
}