#include <SFML/Graphics.hpp>
#include <list>

using namespace sf;

const int H = 9;
const int W = 30;
float DEGTORAD = 0.017453f;

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
	float dx, dy;		//скорость
	FloatRect rect;		//загрузка текстури игрока (координат)
	Sprite sprite;		//загрузка спрайта игрока
	int life;			//живий чи ні
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
		//установка анимації
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


				//переход між уровнями
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

class bullet : public  Entity 
{
public:
	bullet()
	{
		name = "bullet";
	}
	void update()
	{
		int o, l;
		dx = cos(angle*DEGTORAD) * 0.5;
		dy = sin(angle*DEGTORAD) * 0.5;
		x += dx;
		y += dy;
		o = x / 64;
		l = y / 64;
		if (x > 960 || x < 0 || y>576 || y < 0) life = 0;
				if (Level1[l][o] != ' ') life = 0;
	}
};

int main()
{
	RenderWindow window(VideoMode(960, 576), "Kursach game!");
	Clock clock;
	Texture t, st, wood, bonus, enemyt, bullett;
	t.loadFromFile("player.png");
	st.loadFromFile("stone.png");
	wood.loadFromFile("wood.png");
	bonus.loadFromFile("bonus.png");
	enemyt.loadFromFile("enemy.png");
	bullett.loadFromFile("bullet.png");

	Animation sbullet(bullett,0,0,32,32,1,1);

	RectangleShape rectangle(Vector2f(64, 64));

	Sprite map;
	float currentFrame = 0;

	Sprite backgroung;
	std::list<Entity*> entities;

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
					b->settings(sbullet, p.rect.left+24, p.rect.top+24, angle, 10);
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
		p.update(time);
		window.clear(Color::White);

		//фон
		backgroung.setTexture(wood);
		window.draw(backgroung);

		//прорисовка карти
		for (int i = 0; i<H; i++)
			for (int j = 0; j<15; j++)
			{
				if (Level1[i][j] == 'Z') map.setTexture(st);

				if (Level1[i][j] == 'e')  map.setTexture(bonus);

				if (Level1[i][j] == 'o')  map.setTexture(st);

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