#include <SFML/Graphics.hpp>

using namespace sf;

const int H = 9;
const int W = 30;

int CW = 0;

String Level1[H] = {
	"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
	"Z             ZZeee    Z   ZeZZe           eZZ     Z     eeZZ        Z    ZZ             Z",
	"Z             ZZeee  Z Z Z ZeZZ    Z  Z     ZZ     Z     eeZZ        Z Z  ZZ             Z",
	"Z             ZZZZZ  Z Z Z ZeZZ    Z  Z     ZZ     ZZZ   eeZZZ    Z  Z Z  ZZ             Z",
	"Z             oZ  Z  Z Z Z Z oZ    Z  Z     oZ             oZ     Z  Z Z  oZ             Z",
	"Z             ZZ  Z  Z Z Z Z ZZ   ZZZZZZ    ZZ     ZZZ   eeZZZZ   Z ZZ ZZZZZ             Z",
	"Z             ZZ  Z  Z Z Z Z ZZ  ZeeeeeeZ   ZZ     Z     eeZZ     Z Z     ZZ             Z",
	"Z             ZZ     Z   Z   ZZe ZeeeeeeZ  eZZ     Z     eeZZ     Z       ZZ             Z",
	"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
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
		Collision(0);
		rect.top += dy*time;
		Collision(1);

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

	void Collision(int dir)
	{
		for (int i = rect.top / 64; i<(rect.top + rect.height) / 64; i++)
			for (int j = rect.left / 64;  j<(rect.left + rect.width) / 64; j++)
			{
				if (Level1[i][j] == 'Z')
				{
					if ((dx>0) && (dir == 0)) rect.left = j * 64 - rect.width;
					if ((dx<0) && (dir == 0)) rect.left = j * 64 + 64;
					if ((dy>0) && (dir == 1)) rect.top = i * 64 - rect.height;
					if ((dy<0) && (dir == 1)) rect.top = i * 64 + 64; 
				}

				if (Level1[i][j] == 'e')
				{
					Level1[i][j] = ' '; 
				}


				//проверка на дверь
				if (Level1[i][j] == 'o')
				{

					CW += 15;
					rect.left = 70;
					rect.top = 270;
					for (int chh = 0; chh < 9; chh++)
						for (int chw = 0; chw < 15; chw++)
							Level1[chh][chw] = Level1[chh][chw + CW];
				}

			}

	}

};


int main()
{
	RenderWindow window(VideoMode(960, 576), "Kursach game!");
	Clock clock;
	
	Texture t;
	t.loadFromFile("player.png");

	Texture st;
	st.loadFromFile("stone.png");

	Texture wood;
	wood.loadFromFile("wood.png");

	Texture bonus;
	bonus.loadFromFile("bonus.png");
	
	RectangleShape rectangle(Vector2f(64, 64));
	
	Sprite map;
	float currentFrame = 0;

	Sprite backgroung;

	PLAYER p(t);

	while (window.isOpen())
	{
		float time = 20;
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
				
				//загруззка фона
				backgroung.setTexture(wood);
				window.draw(backgroung);

				//прорисовка препядствий
				for (int i = 0; i<H; i++)
					for (int j = 0; j<15; j++)
					{
						if (Level1[i][j] == 'Z') map.setTexture(st);

						if (Level1[i][j] == 'e')  map.setTexture(bonus);

						if (Level1[i][j] == 'o')  map.setTexture(st);

						if (Level1[i][j] == ' ') continue;

						map.setPosition(j* 64 , i * 64);
						window.draw(map);
					}


			window.draw(p.sprite);
		window.display();
	}

	return 0;
}