#include <iostream>
#include <raylib.h>
#include <deque>
#include <cmath>
#include <raymath.h>
#include <vector>
using namespace std;

Color green = { 173,204,96,255 };
Color green1 = { 175,200,95,255 };
Color darkGreen = { 43,51,24,255 };
Color red = { 200,0,0,255 };
Color purple = { 150, 0, 100,255 };
Color colortam = darkGreen;
int cellSize = 30; // Kích thước một ô là 30
int cellCount = 25; // chia thành 25 ô cả ngang + dọc: mỗi ô đánh số từ 0 -> 24
int offset = 75; // Kích thước bù thêm để tạo khu vực chơi 
double lastUpdateTime = 0; /* Thời gian mà chương trình vừa chạy và rắn chưa di chuyển
								(Lần cuối mà rắn di chuyển khi chạy chương trình) là 0 */


// Hàm kiểm tra vị trí của thức ăn có trùng với rắn hay không
bool ElementInDeQue(Vector2 element, deque<Vector2> deque)
{
	for (int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}

// Hàm kiểm tra để giới hạn thời gian rắn di chuyển
bool eventTriggerd(double interval) {
	double currentTime = GetTime();

	/*Kiểm tra khoảng thời gian giữa 2 lần rắn di chuyển */
	if (currentTime - lastUpdateTime >= interval)
	{
		// Đặt lại giá trị mới cho lần rắn di chuyển gần nhất 
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

//Vị trí tạm để dịch chuyển thức ăn ra khỏi phạm vi chơi để không bị trùng vị trí nhiều loại thức ăn và xử lý trường hợp 
//thức ăn bị tàng hình mà rắn vẫn va chạm
Vector2 tam = { 100,100 };

class Snake
{
private:
	float speed;

public:
	Snake() {
		speed = 0.2;
	}
	Snake(float a) {
		speed = a;
	}
	Snake(const Snake& p) {
		speed = p.speed;
	}

	void setSpeed(float k) { speed = k; }
	float getSpeed() { return speed; }
	// Tạo đối tượng rắn bằng các ô nối liền nhau
	deque<Vector2> body = { Vector2{6,9}, Vector2{5,9},Vector2{4,9} };

	// Phương hướng duy chuyển của rắn ban đầu (phải)
	Vector2 direction = { 1,0 };
	bool addSegment = false;
	bool subSegment = false;
	bool subSegmentx2 = false;
	bool addSegmentx2 = false;
	
	void Draw() {
		for (int i = 0; i < body.size(); i++) 
		{
			float x = body[i].x;
			float y = body[i].y;

			// Tạo biến segment lưu giá trị của đối tượng Rectangle 
			Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
			// Vẽ đối tượng rắn lên màn hình là các ô vuông bo tròn góc
			DrawRectangleRounded(segment, 0.5, 6, colortam);
		}
	}

	// Hàm cập nhật chiều dài của rắn sau khi ăn
	void Update()
	{
		
		// Tăng chiều dài của rắn theo hướng di chuyển
		if (addSegment == true)
		{
			// Tăng 1 ô
			body.push_front(Vector2Add(body[0], direction));
			// Tăng 2 ô
			if (addSegmentx2 == true) {
				body.push_front(Vector2Add(body[0], direction));
				addSegmentx2 = false;
				addSegment = false;
			}
			else
				addSegment = false;
		}

		// Giảm chiều dài của rắn theo hướng di chuyển
		if (subSegment == true)
		{
			// Giảm 1 ô
			if (body.size() > 4) {
				body.push_front(Vector2Add(body[0], direction));
				body.pop_back();
				body.pop_back();

				// Giảm 2 ô
				if (subSegmentx2 == true) {
					body.pop_back();
					subSegmentx2 = false;
					subSegment = false;
				}
			}
			else
			{
				subSegment = false;
			}
		}
		// Thay đổi hướng di chuyển của rắn 
		else
		{
			body.push_front(Vector2Add(body[0], direction));
			body.pop_back();
		}
	}

	// Đặt lại giá trì sau khi kết thúc game
	void Reset() {
		body = { Vector2 {6,9}, Vector2{5,9}, Vector2{4,9} };
		direction = { 1,0 };
	}
};

class Food
{
private:
	Vector2 position;
	int index;
public:

	void setIndex(int k) {
		index = k;
	}

	int getIndex() {
		return index;
	}

	void setPosition(Vector2 a) {
		position = a;
	}

	Vector2 getPosition() {
		return position;
	}

	// Mảng hình ảnh tối đa 10 hình ảnh
	Texture2D texture[10];
	Food() {
		// Tải texture - tạo hình ảnh cho đối tượng 
		position = { 10,10 };
		index = 0;
		texture[index] = LoadTexture("Graphics\\core.png");
	}
	
	Food(Vector2 _pos, int _index) {
		position = _pos;
		index = _index;
		texture[index] = LoadTexture("Graphics\\core.png");
	}

	Food(const Food& p) {
		position = p.position;
		index = p.index;
	}

	~Food() {
		// Giải phóng bộ nhớ của đối tượng texture
		UnloadTexture(texture[index]);
	}

	void Draw()
	{
		// Vẽ thức ăn lên màn hình 
		DrawTexture(texture[getIndex()], offset + getPosition().x * cellSize, offset + getPosition().y * cellSize, WHITE);
	}

	// Hàm trả về vị trí random thức ăn khi chưa kiểm tra trùng vị trí
	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x,y };
	}

	// Hàm trả về vị trí random thức ăn sau khi đã kiểm tra trùng vị trí	
	Vector2 GenerateRandomPos(deque <Vector2> snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeQue(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}

	// Phương thức ảo của lớp Food 
	virtual void effect(Snake &snake, int &score, Sound eatSound) {
		snake.addSegment = true;
		score = score + 1;
		PlaySound(eatSound);
	}
};

class Trap : public Food {

public:
	Trap(){
		Vector2 b = { 100,100 };
		setPosition(b);
		setIndex(1);
		texture[getIndex()] = LoadTexture("Graphics\\trap.png");
	}

	Trap(Vector2 _pos, int _index) : Food(_pos, _index) {};
		

	Trap(const Trap& p) : Food(p) {};
	~Trap() {
		UnloadTexture(texture[getIndex()]);
	}

	void effect(Snake &snake, int& score, Sound hitSound) {
		snake.subSegment = true;
		score = score - 1;
		PlaySound(hitSound);
	}

};

class FishBone : public Food {
public:

	FishBone() {
		setPosition(tam);
		setIndex(2);
		texture[getIndex()] = LoadTexture("Graphics\\fishbone.png");
	}

	FishBone(Vector2 _pos, int _index) : Food(_pos, _index) {};

	FishBone(const FishBone& p) : Food(p) {};

	~FishBone() {
		UnloadTexture(texture[getIndex()]);
	}

	void effect(Snake& snake, int &score, Sound coughSound){
		colortam = purple;
		snake.setSpeed(0.6);
		score = score - 1;
		PlaySound(coughSound);
	}
};

class Banana : public Food {


public:
	Banana() {
		setPosition(tam);
		setIndex(3);
		texture[getIndex()] = LoadTexture("Graphics\\banana.png");
	}

	Banana(Vector2 _pos, int _index) : Food(_pos, _index) {};

	Banana(const Banana& p) : Food(p) {};

	~Banana() {
		UnloadTexture(texture[getIndex()]);
	}

	void effect(Snake& snake, int& score, Sound burpSound) {
		snake.addSegment = true;
		snake.addSegmentx2 = true;
		score = score + 2;
		PlaySound(burpSound);
	}
};

class SuperTrap : public Food {

public:
	SuperTrap() {
		setPosition(tam);
		setIndex(4);
		texture[getIndex()] = LoadTexture("Graphics\\supertrap.png");
	}

	SuperTrap(Vector2 _pos, int _index) : Food(_pos, _index) {};

	SuperTrap(const SuperTrap& p) : Food(p) {};

	~SuperTrap() {
		UnloadTexture(texture[getIndex()]);
	}
	float startTime = 0.0f; // Biến lưu trữ thời điểm bắt đầu tàng hình

	void effect(Snake& snake, int& score, Sound screamingSound) {
		snake.subSegment = true;
		snake.subSegmentx2 = true;
		score = score - 2;
		PlaySound(screamingSound);
		// Khôi phục màu gốc sau khi hiệu ứng kết thúc
		colortam = green1;

	};
};

class Drug : public Food {

	public:
		Drug() {
			setPosition(tam);
			setIndex(5);
			texture[getIndex()] = LoadTexture("Graphics\\drug.png");
		}

		Drug(Vector2 _pos, int _index) : Food(_pos, _index) {};

		Drug(const Drug& p) : Food(p) {};

		~Drug() {
			UnloadTexture(texture[getIndex()]);
		}

		void effect(Snake& snake, int& score, Sound happySound) {
			colortam = red;
			snake.setSpeed(0.1);
			score = abs(score) * 2;
			PlaySound(happySound);
		}
	};

class GameBoard
	{
		Food food;
		Trap trap;
		FishBone fishbone;
		Banana banana;
		SuperTrap supertrap;
		Drug drug;
	public:
		Snake snake;
		bool running = true;
		int score = 0;
		Sound eatSound, failSound, hitSound, burpSound, coughSound, screamingSound, happySound;

		// Hàm khởi tạo để tạo âm thanh cho game
		GameBoard()
		{
			InitAudioDevice();
			eatSound = LoadSound("Sounds\\eat.mp3");
			failSound = LoadSound("Sounds\\fail.mp3");
			hitSound = LoadSound("Sounds\\hit.mp3");
			burpSound = LoadSound("Sounds\\burp.mp3");
			coughSound = LoadSound("Sounds\\cough.mp3");
			screamingSound = LoadSound("Sounds\\aaa.mp3");
			happySound = LoadSound("Sounds\\drug.mp3");
		}

		// Hàm hủy để xóa âm thanh 
		~GameBoard()
		{
			UnloadSound(eatSound);
			UnloadSound(failSound);
			UnloadSound(hitSound);
			UnloadSound(burpSound);
			UnloadSound(coughSound);
			UnloadSound(screamingSound);
			UnloadSound(happySound);
			CloseAudioDevice();
		}

		// Hàm vẽ rắn và thức ăn lên màn hình
		void Draw()
		{
			snake.Draw();
			switch (food.getIndex()) {
			case 0: {
				food.Draw();
				break;
			}
			case 1: {
				trap.Draw();
				break;
			}
			case 2: {
				fishbone.Draw();
				break;
			}
			case 3: {
				banana.Draw();
				break;
			}
			case 4: {
				supertrap.Draw();
			}
			case 5: {
				drug.Draw();
			}
			}
		}

		// Hàm cập nhật sau khi rắn va chạm với thức ăn hoặc lưới
		void Update()
		{
			if (running)
			{
				snake.Update();
				CheckCollisionWithFood();
				CheckCollisionWithEdges();
				CheckCollisioWithTail();
				CheckLength();

			}
		}

		// Kiểm tra va chạm giữa rắn và các loại thức ăn
		void CheckCollisionWithFood()
		{

			// Kiểm tra vị trí trùng nhau giữa rắn và trái táo
			if (Vector2Equals(snake.body[0], food.getPosition()))
			{
				// Tạo mảng gồm các loại thức ăn kế tiếp sau khi rắn ăn táo (1: Bẫy, 2: xương cá , 3: chuối, 4: )
				ResetColor();
				ResetSpeed();
				int numbers[] = { 1, 2, 3, 4, 5 };
				food.setPosition(tam); // ẩn đi vị trí của táo
				food.setIndex(numbers[GetRandomValue(0,4)]); // Chọn ngẫu nhiên loại thức ăn từ mảng 
				cout << "Toi la tao day" << endl;
				switch (food.getIndex())
				{
				case 1: {
					trap.setPosition(trap.GenerateRandomPos(snake.body));
					break;
				}
				case 2: {
					fishbone.setPosition(fishbone.GenerateRandomPos(snake.body));
					break;
				}

				case 3: {
					banana.setPosition(banana.GenerateRandomPos(snake.body));
					break;
				}
				case 4: {
					supertrap.setPosition(supertrap.GenerateRandomPos(snake.body));
					break;
				}
				case 5: {
					drug.setPosition(drug.GenerateRandomPos(snake.body));
					break;
				}
				default:
					break;
				}
				// Tạo hiệu ứng khi rắn ăn thức ăn táo 
				food.effect(snake, score, eatSound);
			}

			// Kiểm tra va chạm với bẫy
			if (Vector2Equals(snake.body[0], trap.getPosition()))
			{
				// Dịch chuyển vị trí thức ăn sau khi rắn đạp bẫy (0: táo, 2: xương cá, 3: chuối, 4: siêu bẫy, 5: ma túy)
				ResetColor();
				ResetSpeed();
				int numbers[] = { 0, 2, 3, 4, 5 };
				trap.setPosition(tam);
				food.setIndex(numbers[GetRandomValue(0, 4)]);
				cout << "Toi la bay day" << endl;
				switch (food.getIndex())
				{
				case 0: {
					food.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 2: {
					fishbone.setPosition(fishbone.GenerateRandomPos(snake.body));
					break;
				}

				case 3: {
					banana.setPosition(banana.GenerateRandomPos(snake.body));
					break;
				}
				case 4: {
					supertrap.setPosition(supertrap.GenerateRandomPos(snake.body));
					break;
				}
				case 5: {
					drug.setPosition(drug.GenerateRandomPos(snake.body));
					break;
				}
				}
				// Tạo hiệu ứng khi rắn đạp bẫy
				trap.effect(snake, score, hitSound);
			}
			// Kiểm tra va chạm với chuối
			else if (Vector2Equals(snake.body[0], banana.getPosition()))
			{
				ResetColor();
				ResetSpeed();
				// Dịch chuyển vị trí thức ăn sau khi rắn ăn chuối (0: táo, 1: bẫy, 2: xương cá, 4: siêu bẫy, 5: ma túy)
				int numbers[] = { 0, 1, 2, 4, 5 };
				banana.setPosition(tam);
				food.setIndex(numbers[GetRandomValue(0, 4)]);
				cout << "Toi la chuoi day" << endl;
				switch (food.getIndex())
				{
				case 0: {
					food.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 2: {
					fishbone.setPosition(fishbone.GenerateRandomPos(snake.body));
					break;
				}

				case 1: {
					trap.setPosition(trap.GenerateRandomPos(snake.body));
					break;
				}
				case 4: {
					supertrap.setPosition(supertrap.GenerateRandomPos(snake.body));
					break;
				}
				case 5: {
					drug.setPosition(drug.GenerateRandomPos(snake.body));
					break;
				}
				}
				// Tạo hiệu ứng khi rắn ăn chuối
				banana.effect(snake, score, burpSound);
			}

			// Kiểm tra va chạm với siêu bẫy
			else if (Vector2Equals(snake.body[0], supertrap.getPosition()))
			{
				ResetColor();
				ResetSpeed();
				// Dịch chuyển vị trí thức ăn sau khi rắn đạp bẫy (0: táo, 1: bẫy, 2: xương cá, 3: chuối, 5: ma túy)
				int numbers[] = { 0, 1, 2, 3, 5 };
				supertrap.setPosition(tam);
				food.setIndex(numbers[GetRandomValue(0, 4)]);
				cout << "Toi la sieu bay day" << endl;
				switch (food.getIndex())
				{
				case 0: {
					food.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 2: {
					fishbone.setPosition(fishbone.GenerateRandomPos(snake.body));
					break;
				}

				case 3: {
					banana.setPosition(banana.GenerateRandomPos(snake.body));
					break;
				}
				case 1: {
					trap.setPosition(trap.GenerateRandomPos(snake.body));
					break;
				}
				case 5: {
					drug.setPosition(drug.GenerateRandomPos(snake.body));
					break;
				}
				}
				// Tạo hiệu ứng khi rắn đạp bẫy
				supertrap.effect(snake, score, screamingSound);
			}

			// Kiểm tra va chạm với xương cá
			else if (Vector2Equals(snake.body[0], fishbone.getPosition()))
			{
				// Dịch chuyển vị trí thức ăn sau khi rắn mắc xương cá (0: táo, 1: bẫy, 3: chuối, 4: siêu bẫy, 5: ma túy)
				int numbers[] = { 0, 1, 3, 4, 5 };
				fishbone.setPosition(tam);
				food.setIndex(numbers[GetRandomValue(0, 4)]);
				cout << "Toi la xuong ca day" << endl;
				switch (food.getIndex())
				{
				case 0: {
					food.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 3: {
					banana.setPosition(banana.GenerateRandomPos(snake.body));
					break;
				}
				case 1: {
					trap.setPosition(trap.GenerateRandomPos(snake.body));
					break;
				}
				case 4: {
					supertrap.setPosition(supertrap.GenerateRandomPos(snake.body));
					break;
				}
				case 5: {
					drug.setPosition(drug.GenerateRandomPos(snake.body));
					break;
				}
				}
				// Tạo hiệu ứng khi rắn mắc xương cá
				fishbone.effect(snake, score, coughSound);
			}

			// Kiểm tra va chạm với thuốc
			else if (Vector2Equals(snake.body[0], drug.getPosition())) {
				// Dịch chuyển vị trí thức ăn sau khi rắn đạp bẫy (0: táo, 1: bẫy, 2: xương cá, 4: siêu bẫy, 3: chuối)
				int numbers[] = { 0, 1, 2, 3, 4 };
				drug.setPosition(tam);
				food.setIndex(numbers[GetRandomValue(0, 4)]);
				cout << "Toi la ma tuy day" << endl;
				switch (food.getIndex())
				{
				case 0: {
					food.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 2: {
					fishbone.setPosition(fishbone.GenerateRandomPos(snake.body));
					break;
				}

				case 1: {
					trap.setPosition(food.GenerateRandomPos(snake.body));
					break;
				}
				case 4: {
					supertrap.setPosition(supertrap.GenerateRandomPos(snake.body));
					break;
				}
				case 3: {
					banana.setPosition(banana.GenerateRandomPos(snake.body));
					break;
				}
				}
				// Tạo hiệu ứng khi rắn phê
				drug.effect(snake, score, happySound);
			}
		}

		// Kiểm tra va chạm với khung hình 
		void CheckCollisionWithEdges()
		{
			if (snake.body[0].x == cellCount || snake.body[0].x == -1)
			{
				GameOver();
			}
			if (snake.body[0].y == cellCount || snake.body[0].y == -1)
			{
				GameOver();
			}
		}

		// Kiểm tra va chạm với đuôi
		void CheckCollisioWithTail()
		{
			// Tạo một bản sao deque mới từ rắn 
			deque <Vector2> headlessBody = snake.body;

			// Xóa phần từ đầu tiên của rắn vì rắn không thể tự va chạm với đầu của nó 
			headlessBody.pop_front();

			// Kiểm tra va chạm giữa đầu rắn và các bộ phận còn lại 
			if (ElementInDeQue(snake.body[0], headlessBody))
			{
				GameOver();
			}
		}

		// Kiểm tra chiều dài tối thiểu
		void CheckLength() {
			if (snake.body.size() < 2)
				GameOver();
		}

		// Đặt lại màu sắc ban đầu khi  rắn ăn các thức ăn không có hiệu ứng đổi màu
		void ResetColor() {
			if (food.getIndex() != 4 || food.getIndex() != 5)
				colortam = darkGreen;
		}

		// Đặt lại tốc độ ban đầu khi rắn ăn các thức ăn không thay đổi tốc độ
		void ResetSpeed() {
			if (food.getIndex() != 4 || food.getIndex() != 5)
				snake.setSpeed(0.2);
		}

		// Kiểm tra game kết thúc
		void GameOver()
		{
			food.setIndex(0);
			snake.Reset();
			food.setPosition(food.GenerateRandomPos(snake.body));
			trap.setPosition(tam);
			fishbone.setPosition(tam);
			banana.setPosition(tam);
			snake.setSpeed(0.2);
			colortam = darkGreen;
			running = false;
			score = 0;
			PlaySound(failSound);
		}

	};

int main() {

		// Tạo cửa sổ window có tiêu đề là "Snake Game"
		InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Snake Game");

		// Cài đặt cấu hình FPS cho mọi máy 
		SetTargetFPS(60);

		GameBoard game;

		while (WindowShouldClose() == false)
		{
			BeginDrawing();

			// Kiểm tra điều kiện thời gian rắn di chuyển không quá thời gian quy định
			if (eventTriggerd(game.snake.getSpeed()))
			{
				game.Update();
			}

			// Cài đặt phím di chuyển cho rắn
			if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) /* Khi nhấn phím up thì direction.y phải != 1 (vì - 1 là rắn di chuyển 1 ô lên)
																	(còn 1 là rắn di chuyển 1 ô xuống) là để không quay đầu ngược lại khi đang chạy lên
																	Hoặc có thể hiểu rắn đang đi lên thì y phải giảm 1 , chứ không được tăng 1*/
			{
				game.snake.direction = { 0,-1 };
				game.running = true;
			}

			if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
			{
				game.snake.direction = { 0,1 };
				game.running = true;
			}

			if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
			{
				game.snake.direction = { -1 ,0 };
				game.running = true;
			}

			if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
			{
				game.snake.direction = { 1,0 };
				game.running = true;
			}

			/*Hàm vẽ background thành màu xanh lá */
			ClearBackground(green);

			// Hàm vẽ khu vực để rắn di chuyển 
			DrawRectangleLinesEx({ (float)offset - 5,(float)offset - 5,(float)cellSize * cellCount + 10 ,(float)cellSize * cellCount + 10 }, 5, darkGreen);

			// Hàm vẽ tiêu đề game
			DrawText("Snake Game", offset - 5, 20, 40, darkGreen);

			// Hàm vẽ điểm lên màn hình
			DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);

			// Vẽ rắn và thức ăn lên màn hình 
			game.Draw();
			EndDrawing();
		}

		CloseWindow();
	}