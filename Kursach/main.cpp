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
	"Z             ZZeee    s   seZZe           eZZ     s     eeZZ        s    ZZ             Z",
	"Z             ZZeee  s s s seZZ    s  s     ZZ     s     eeZZ        s s  ZZ             Z",
	"Z             ZZsss  s s s seZZ    s  s     ZZ     sss   eeZZs    s  s s  ZZ             Z",
	"Z             oZ  s  s s s s oZ    s  s     oZ             oZ     s  s s  oZ             Z",
	"Z             ZZ  s  s s s s ZZ   ssssss    ZZ     sss   eeZZss   s ss sssZZ             Z",
	"Z             ZZ  s  s s s s ZZ  seeeeees   ZZ     s     eeZZ     s s     ZZ             Z",
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
	int life;			//живий чи н≥
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
		if (Level1[int(y / 64)][int(x / 64)] != ' ') dx = dx*-1;
		if (Level1[int(y / 64)][int(x / 64)] != ' ') dy = dy*-1;
	}
};

bool isCollide(Entity *a, Entity *b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y) <
		(a->R + b->R)*(a->R + b->R);
}

int main()
{
	RenderWindow window(VideoMode(960, 576), "Kursach game!");
	menu(window);//вызов меню
	Clock clock;
	srand(time(0));
	Texture t, st, wood, bonus, enemyt, bullett, wall, door;
	t.loadFromFile("player.png");
	st.loadFromFile("stone.png");
	wall.loadFromFile("wall.png");
	door.loadFromFile("door.png");
	wood.loadFromFile("wood.png");
	bonus.loadFromFile("bonus.png");
	enemyt.loadFromFile("enemy.png");
	bullett.loadFromFile("bullet.png");
	FloatRect recte;

	Animation sbullet(bullett, 0, 0, 32, 32, 1, 1);
	Animation senemy(enemyt, 0, 0, 32, 32, 1, 1);
	RectangleShape rectangle(Vector2f(64, 64));

	Sprite map;
	float currentFrame = 0;

	Sprite backgroung;
	int checkx = 0, checky = 0;
	std::list<Entity*> entities;
	//создание врагов
	for (int i = 0; i < 5; i++)
	{
		checkx = rand() % 800 + 64;
		checky = rand() % 476 + 64;
		//	if (Level1[checkx / 64][checky / 64] != ' ') { checkx += 64; checky += 64; }

		enemy *a = new enemy();
		a->settings(senemy, checkx, checky, 270, 1);
		entities.push_back(a);

	}

	PLAYER p(t);
	int angle = 0;
	while (window.isOpen())
	{
		float time = 1;
		Event event;
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

		for (auto a : entities)
			for (auto b : entities)
				if (a->name == "enemy"&&b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false; b->life = false;
					}

		p.update(time);
		window.clear(Color::White);

		//фон
		backgroung.setTexture(wood);
		window.draw(backgroung);

		//прорисовка карти
		for (int i = 0; i<H; i++)
			for (int j = 0; j<15; j++)
			{
				if (Level1[i][j] == 'Z') map.setTexture(wall);

				if (Level1[i][j] == 'e')  map.setTexture(bonus);

				if (Level1[i][j] == 'o')  map.setTexture(door);

				if (Level1[i][j] == 's') map.setTexture(st);

				if (Level1[i][j] == ' ') continue;

				map.setPosition(j * 64, i * 64);
				window.draw(map);
			}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity *e = *i;
			e->update();
			e->anim.update();
			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}

		for (auto i : entities) i->draw(window);

		window.draw(p.sprite);
		window.display();
	}

	return 0;
}