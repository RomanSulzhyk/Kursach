#include <SFML/Graphics.hpp>
#include <list>
#include <stdio.h>

using namespace sf;

const int H = 9;
const int W = 30;
float DEGTORAD = 0.017453f;

int CW = 0;

String Level1[H] = {
	"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
	"Z            vZZeee v  s   seZZe           eZZ     s     eeZZ        s    ZZ             Z",
	"Z             ZZeee  s s s seZZ    s  s     ZZ     s     eeZZ        s s  ZZ             Z",
	"Z             ZZsss  s s s seZZ    s  s     ZZ     sss   eeZZs    s  s s  ZZ             Z",
	"Z             ZZ  s  s s s s ZZ    s  s     ZZ             ZZ     s  s s  ZZ             Z",
	"Z             ZZ  s  s s s s ZZ   ssssss    ZZ     sss   eeZZss   s ss sssZZ             Z",
	"Z             ZZ vs  s s s s ZZ  seeeeees   ZZ     s     eeZZ     s s     ZZ             Z",
	"Z             ZZ     s   s   ZZe seeeeees  eZZ     s     eeZZ     s       ZZ             Z",
	"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ",
};

class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	std::vector<IntRect> frames;

	Animation() {}
	Animation(Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
		Frame = 0;
		speed = Speed;

		for (int i = 0; i < count; i++)
			frames.push_back(IntRect(x + i*w, y, w, h));
		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);

	}
	void update()
	{
		Frame += 0.01*speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
	}
};

class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity() { life = 1; }

	void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1)
	{
		x = X; y = Y; anim = a;
		angle = Angle; R = radius;
	}

	virtual void update() {};

	void draw(RenderWindow &window)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		window.draw(anim.sprite);
	}

};

class PLAYER {
public:
	float dx, dy;		//скорость
	FloatRect rect;		//загрузка текстури игрока (координат)
	Sprite sprite;		//загрузка спрайта игрока
	bool life = 1;			//живий чи н≥
	float currentFrame;	//поточний кадр игрока


						//конструктор
	PLAYER(Texture &image)
	{
		sprite.setTexture(image);
		rect = FloatRect(0, 0, 48, 48);
		dx = dy = 0.1;
		currentFrame = 0;
		rect.left = 70;
		rect.top = 270;
		life = 1;
	}
	void update(float time)
	{
		rect.left += dx * time;
		Collision(0);
		rect.top += dy*time;
		Collision(1);

		currentFrame += 0.01*time;
		if (currentFrame > 3) currentFrame -= 3;
		//установка анимац≥њ
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
			for (int j = rect.left / 64; j<(rect.left + rect.width) / 64; j++)
			{
				if (Level1[i][j] == 'Z' || Level1[i][j] == 's')
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


				//переход м≥ж уровн¤ми
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

class bullet : public  Entity
{
public:
	bullet()
	{
		name = "bullet";
	}
	void update()
	{
		dx = cos(angle*DEGTORAD) * 0.5;
		dy = sin(angle*DEGTORAD) * 0.5;
		x += dx;
		y += dy;
		if (x > 960 || x < 0 || y>576 || y < 0) life = 0;
		if (Level1[int(y / 64)][int(x / 64)] != ' ') life = 0;
	}
};

class enemy : public Entity {
public:
	enemy()
	{
		dx = 0.05;
		dy = 0.05;
		name = "enemy";
	}
	void update()
	{
		x += dx;
		y += dy;
		if (Level1[int(y / 64)][int(x / 64)] != ' ') dx *= -1;
		if (Level1[int(y / 64)][int(x / 64)] != ' ') dy *= -1;
	}
};


//Функции обработки столкновений и прорисовки меню
bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y) <
		(a->R + b->R)*(a->R + b->R);
}

bool Playerenemy(Entity *a, PLAYER p)
{
	FloatRect rectenemy;
	rectenemy = FloatRect(a->x, a->y, 32, 32);
	return (p.rect.intersects(rectenemy));
}

void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/111.png");
	menuTexture2.loadFromFile("images/222.png");
	menuTexture3.loadFromFile("images/333.png");
	aboutTexture.loadFromFile("images/about.png");
	menuBackground.loadFromFile("images/Penguins.jpg");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(410, 30);
	menu2.setPosition(390, 90);
	menu3.setPosition(430, 150);
	menuBg.setPosition(0, 0);

	//////////////////////////////МЕНЮ///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(129, 181, 221));

		if (IntRect(410, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(390, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(430, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3) { window.close(); isMenu = false; }

		}

		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
}


int main()
{
	RenderWindow window(VideoMode(960, 576), "Kursach game!");
	menu(window);//вызов меню
	Clock clock;
	srand(time(0));
	Texture t, st, wood, bonus, enemyt, bullett, wall, door;
	t.loadFromFile("images/player.png");
	st.loadFromFile("images/stone.png");
	wall.loadFromFile("images/wall.png");
	door.loadFromFile("images/door.png");
	wood.loadFromFile("images/wood.png");
	bonus.loadFromFile("images/bonus.png");
	enemyt.loadFromFile("images/enemy.png");
	bullett.loadFromFile("images/bullet.png");

	Animation sbullet(bullett, 0, 0, 32, 32, 1, 32);
	Animation senemy(enemyt, 0, 0, 32, 32, 1, 32);
	RectangleShape rectangle(Vector2f(64, 64));
	Sprite map;
	Sprite backgroung;
	float currentFrame = 0;
	int cv = 0;
	int angle = 0;
	std::list<Entity*> entities;
	PLAYER p(t);
	while (window.isOpen())
	{
		float time = 1;
		Event event;

		//проверка на закрытие окна.
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Space)
				{
					bullet *b = new bullet();
					b->settings(sbullet, p.rect.left + 24, p.rect.top + 24, angle, 10);
					entities.push_back(b);
				}
			}
		}
		//движение персонажа
		if (Keyboard::isKeyPressed(Keyboard::A))
		{
			p.dx = -0.1; angle = 180;
		}
		if (Keyboard::isKeyPressed(Keyboard::D))
		{
			p.dx = 0.1; angle = 0;
		}
		if (Keyboard::isKeyPressed(Keyboard::W))
		{
			p.dy = -0.1; angle = 270;
		}
		if (Keyboard::isKeyPressed(Keyboard::S))
		{
			p.dy = 0.1;; angle = 90;
		}
		if (Keyboard::isKeyPressed(Keyboard::LShift))
		{
			time = 3;
		}
		
		//начало прорисовки
		window.clear(Color::White);
		backgroung.setTexture(wood);
		window.draw(backgroung);

		//прорисовка карти и инициализация врагов
		for (int i = 0; i<H; i++)
			for (int j = 0; j<15; j++)
			{
				if (Level1[i][j] == 'Z') map.setTexture(wall);

				if (Level1[i][j] == 'e')  map.setTexture(bonus);

				if (Level1[i][j] == 'o')  map.setTexture(door); 

				if (Level1[i][j] == 's') map.setTexture(st);

				if (Level1[i][j] == 'v')
				{
					Level1[i][j] = ' '; enemy *a = new enemy(); a->settings(senemy, j * 64, i * 64, 270, 1); entities.push_back(a); cv++;
				}

				if (Level1[i][j] == ' ') continue;

				map.setPosition(j * 64, i * 64);
				window.draw(map);
			}

		//проверка на столкновения врагов с пулями и героя с врагами.
		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "enemy"&&b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false; b->life = false; cv--;
					}

				if (a->name == "enemy") if (Playerenemy(a, p)) p.sprite.setColor(Color::Red);
			}
		
		//динамическое удаление пуль и врагов.
		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;
			e->update();
			e->anim.update();
			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}
		
		//создание дверей если все враги убиты
		if (cv == 0) Level1[4][14] = 'o';
		//прорисовка всех врагов и пуль
		for (auto i : entities) i->draw(window);

		//функции оновление информации о персонаже.
		p.update(time);
		window.draw(p.sprite);
		window.display();
	}

	return 0;
}