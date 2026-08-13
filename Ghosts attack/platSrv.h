#pragma once

#ifdef PLATSRV_EXPORTS 
#define PLATSRV_API __declspec(dllexport)
#else
#define PLATSRV_API __declspec(dllimport)
#endif

#include <random>
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr float ground_speed{ 1.0f };

constexpr float LINE_EPSILON{ 0.001f };

constexpr D2D1_RECT_F FULL_SCREEN{ 0, 0, scr_width, scr_height };
constexpr D2D1_RECT_F GAME_SCREEN{ 0, sky, scr_width, ground };

constexpr int ERR_PTR{ 5001 };
constexpr int ERR_INDEX{ 5002 };
constexpr int ERR_PARAM{ 5003 };
constexpr int ERR_UNK{ 5004 };

constexpr int MAX_FIELD_ROWS{ 16 };
constexpr int MAX_FIELD_COLS{ 22 };

enum class dirs { up = 0, down = 1, left = 2, right = 3, up_left = 4, up_right = 5, down_left = 6, down_right = 7, stop = 8 };
enum class evils { brain = 0, dervish = 1, ghost = 2, soul = 3 };
enum class assets { armor = 0, life = 1, shot = 2 };
enum class tiles { grass = 0, grass_blue = 1, grass_red = 2, grass_dirt = 3, dirt = 4 };
enum class obstacles { tree1 = 0, tree2 = 1, tree3 = 2, boulder = 3, rock = 4};
enum class actions { move = 0, attack = 1, patrol = 2 };

namespace dll
{
	struct PLATSRV_API TILE
	{
		tiles type{ tiles::grass };
		
		D2D1_RECT_F rect{};

		float delay{ 0 };
	};
	
	struct PLATSRV_API GRID_COORD
	{
		int row{ 0 };
		int col{ 0 };
	};

	struct PLATSRV_API FADING
	{
		assets type = assets::armor;

		D2D1_RECT_F rect{};

		float opacity()
		{
			--delay;
			if (delay <= 0)
			{
				_opacity -= 0.1f;
				delay = 20;
			}

			return _opacity;
		}

	private:
		int delay = 20;
		float _opacity{ 1.0f };
	};

	class PLATSRV_API EXCEPTION
	{
	private:
		int _type{ 0 };

	public:
		EXCEPTION(int what);

		const wchar_t* eGet()const;
	};

	template<typename T> class BAG
	{
	private:
		T* mPtr{ nullptr };
		size_t max_size{ 1 };
		size_t next_pos{ 0 };

	public:

		BAG()
		{
			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			if (!mPtr)throw EXCEPTION(ERR_PTR);
		}
		BAG(size_t lenght)
		{
			max_size = lenght;
			mPtr = reinterpret_cast<T*>(calloc(lenght, sizeof(T)));

			if (!mPtr)throw EXCEPTION(ERR_PTR);
		}
		BAG(BAG& other)
		{
			if (!other.mPtr)throw EXCEPTION(ERR_PARAM);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				
				if (!mPtr)throw EXCEPTION(ERR_PARAM);
				else if (next_pos > 0)for (size_t i = 0; i < next_pos; ++i)mPtr[i] = other.mPtr[i];
			}
		}
		BAG(BAG&& other)
		{
			if (!other.mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;
				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}
		}

		~BAG()
		{
			free(mPtr);
		}

		bool empty() const
		{
			return(next_pos == 0);
		}
		size_t size() const
		{
			return next_pos;
		}
		size_t capacity() const
		{
			return max_size;
		}

		BAG& operator = (BAG& other)
		{
			if (!other.mPtr)throw EXCEPTION(ERR_PARAM);
			else
			{
				free(mPtr);

				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (!mPtr)throw EXCEPTION(ERR_PARAM);
				else if (next_pos > 0)for (size_t i = 0; i < next_pos; ++i)mPtr[i] = other.mPtr[i];
			}

			return *this;
		}
		BAG& operator = (BAG&& other)
		{
			if (!other.mPtr)throw EXCEPTION(ERR_PARAM);
			else
			{
				free(mPtr);

				max_size = other.max_size;
				next_pos = other.next_pos;
				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}

			return *this;
		}

		T& operator[](size_t index)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else if (index < 0 || index >= next_pos)throw EXCEPTION(ERR_PARAM);
			
			return mPtr[index];
		}
		T& front()
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else if (next_pos == 0)throw EXCEPTION(ERR_PARAM);

			return *mPtr;
		}
		T& back()
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else if (next_pos == 0)throw EXCEPTION(ERR_PARAM);

			return mPtr[next_pos - 1];
		}

		void push_back(T element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					
					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else mPtr[next_pos] = element;
					
					++next_pos;
				}
			}
		}
		void push_back(T* element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else mPtr[next_pos] = *element;

					++next_pos;
				}
			}
		}

		void push_front(T element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t ind = next_pos; ind > 0; --ind)mPtr[ind] = mPtr[ind - 1];
					*mPtr = element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t ind = next_pos; ind > 0; --ind)mPtr[ind] = mPtr[ind - 1];
						*mPtr = element;
						++next_pos;
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t ind = next_pos; ind > 0; --ind)mPtr[ind] = mPtr[ind - 1];
					*mPtr = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));

					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t ind = next_pos; ind > 0; --ind)mPtr[ind] = mPtr[ind - 1];
						*mPtr = *element;
						++next_pos;
					}
				}
			}
		}

		void erase(size_t index)
		{
			if (!mPtr) throw EXCEPTION(ERR_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(ERR_PARAM);

				for (size_t ind = index; ind < next_pos - 1; ++ind)mPtr[ind] = mPtr[ind + 1];
				--next_pos;
			}
		}
		void clear()
		{
			max_size = 1;
			next_pos = 0;

			free(mPtr);

			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}

		void insert(size_t index, T element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(ERR_PARAM);

				if (next_pos + 1 <= max_size)
				{
					for (size_t ind = next_pos; ind > index; --ind)mPtr[ind] = mPtr[ind - 1];
					mPtr[index] = element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t ind = next_pos; ind > index; --ind)mPtr[ind] = mPtr[ind - 1];
						mPtr[index] = element;
						++next_pos;
					}
				}
			}
		}
		void insert(size_t index, T* element)
		{
			if (!mPtr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (index < 0 || index >= next_pos)throw EXCEPTION(ERR_PARAM);

				if (next_pos + 1 <= max_size)
				{
					for (size_t ind = next_pos; ind > index; --ind)mPtr[ind] = mPtr[ind - 1];
					mPtr[index] = *element;
					++next_pos;
				}
				else
				{
					++max_size;
					mPtr = reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T)));
					if (!mPtr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t ind = next_pos; ind > index; --ind)mPtr[ind] = mPtr[ind - 1];
						mPtr[index] = *element;
						++next_pos;
					}
				}
			}
		}
	};

	class PLATSRV_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();
		~RANDIT();

		int operator()(int min, int max);
		float operator()(float min, float max);
	};

	class PLATSRV_API PROTON
	{
	protected:
		float _width{ 0 };
		float _height{ 0 };

		D2D1_RECT_F _rect{};

	public:
		D2D1_POINT_2F start{};
		D2D1_POINT_2F end{};
		D2D1_POINT_2F center{};

		float x_rad{ 0 };
		float y_rad{ 0 };

		PROTON();
		PROTON(float sx, float sy);
		PROTON(float sx, float sy, float first_width, float first_height);
		PROTON(D2D1_POINT_2F start_point, float first_width, float first_height);

		virtual ~PROTON() {};

		float get_width()const;
		float get_height()const;

		D2D1_RECT_F get_rect()const;

		void new_width(float new_width);
		void new_height(float new_height);
		void new_dims(float new_width, float new_height);

		void set_edges();
	};

	class PLATSRV_API FIELD
	{
	private:
		TILE FieldArray[MAX_FIELD_ROWS][MAX_FIELD_COLS]{};
		
		float _speed{ ground_speed };

		RANDIT randerer{};

		void add_tiles(dirs towhere);

	public:

		FIELD();

		void move(float gear, dirs dir);

		TILE get_tile(int row, int col) const;

		GRID_COORD get_coord(float x, float y) const;
		GRID_COORD get_coord(D2D1_POINT_2F point) const;
	};

	class PLATSRV_API HERO :public PROTON
	{
	private:
		float _speed{ 3.0f };

		int frame = 0;
		int max_frames = 1;
		int frame_delay = 20;
		int max_frame_delay = 20;

		bool hor_dir = false;
		bool ver_dir = false;

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		float slope{ 0 };
		float intercept{ 0 };

		HERO(float _sx, float _sy);

	public:
		float angle{ 0 };

		int lifes = 150;
		int damage = 20;
		int armor = 1;

		void set_path(float targ_x, float targ_y);

		float get_target_x()const;
		float get_target_y()const;

		int get_frame();

		void move(float gear);
		
		void set_view_angle();

		void Release();

		static HERO* create(float sx, float sy);
	};

	class PLATSRV_API SHOT :public PROTON
	{
	private:
		float speed = { 8.0f };

		bool hor_dir{ false };
		bool ver_dir{ false };

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		float slope{ 0 };
		float intercept{ 0 };

		SHOT(float _sx, float _sy, float _ex, float _ey);

	public:

		int damage{ 0 };

		bool move(float gear);

		void Release();

		SHOT* create(float sx, float sy, float ex, float ey);
	};

	class PLATSRV_API OBSTACLE :public PROTON
	{
	private:
		float _speed{ ground_speed };

		OBSTACLE(obstacles _type, float _sx, float _sy);

	public:
		obstacles type{ obstacles::tree1 };

		bool move(float gear, dirs dir);
		void Release();

		static OBSTACLE* create(obstacles my_type, float sx, float sy);
	};

	class PLATSRV_API EVILS :public PROTON
	{
	private:

		RANDIT _randerer{};
		int frame{ 0 };
		int max_frames{ 0 };
		int frame_delay{ 0 };
		int max_frame_delay{ 0 };

		int attack_delay{ 0 };
		int max_attack_delay{ 0 };

		float speed{ 0 };

		bool hor_dir = false;
		bool ver_dir = false;

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		float slope{ 0 };
		float intercept{ 0 };

		float final_target_x{ 0 };
		float final_target_y{ 0 };
		bool obstacle_on_path{ false };

		float view_range{ 0 };

		EVILS(evils what, float _sx, float _sy);

	public:
		evils type{};
		actions action{ actions::patrol };
		
		int lifes{ 0 };
		int damage{ 0 };
		int armor{ 0 };

		void move(float gear);
		void set_path(float targ_x, float targ_y);

		float get_target_x()const;
		float get_target_y()const;

		int get_frame();
		int attack();

		void Release();

		actions AIMove(BAG<D2D1_POINT_2F>& AssetsCenters, BAG<D2D1_RECT_F>& ObstBag, D2D1_POINT_2F hero_center);

		static EVILS* create(evils what, float sx, float sy);
	};

	// FUNCTIONS *******************************

	bool PLATSRV_API Intersect(D2D1_RECT_F first, D2D1_RECT_F second);
	bool PLATSRV_API Intersect(D2D1_POINT_2F first_center, D2D1_POINT_2F second_center, float first_xrad, float second_xrad,
		float first_yrad, float second_yrad);

	float PLATSRV_API Distance(D2D1_POINT_2F first, D2D1_POINT_2F second);
	
	void PLATSRV_API Sort(BAG<D2D1_POINT_2F>& Bag, D2D1_POINT_2F ref);

	void PLATSRV_API Sort(BAG<D2D1_RECT_F>& Bag, D2D1_POINT_2F ref);

	bool PLATSRV_API Point_on_line(D2D1_POINT_2F start_line, D2D1_POINT_2F end_line, D2D1_POINT_2F check_point);

	bool PLATSRV_API Line_box_intersect(D2D1_RECT_F Box, float slope, float intercept);
}