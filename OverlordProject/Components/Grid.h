#pragma once

class BombermanCharacter;
class MeshFilter;
class ModelAnimator;

struct GridCell
{
public:
	GridCell(XMFLOAT3 bottomLeft, XMFLOAT3 topRight)
		:bottomLeft(bottomLeft),
		topRight(topRight)
	{
		center = XMFLOAT3((bottomLeft.x + topRight.x) / 2.f, bottomLeft.y, (bottomLeft.z + topRight.z) / 2.f);
	}

	//Helper function in case you need to know if it exists 
	bool Contains(const GameObject* gameObj) const
	{
		const auto it = std::find(objects.begin(), objects.end(), gameObj);

		return !(it == objects.end());
	}

	bool Contains(const std::wstring& objectTag) const
	{
		auto isSameTag = [&](const GameObject* obj)
		{
			return obj->GetTag() == objectTag;
		};

		return std::ranges::any_of(objects, isSameTag);
	}

	void Remove(const GameObject* gameObj)
	{
		objects.erase(std::ranges::remove(objects, gameObj).begin(), objects.end());
	}

	void Add(GameObject* gameObj)
	{
		objects.push_back(gameObj);
	}

	XMFLOAT3 bottomLeft{};
	XMFLOAT3 topRight{};
	XMFLOAT3 center{};

	std::vector<GameObject*> objects{};

	bool isValid{ true };
};

struct InvalidCell final : public GridCell
{
		InvalidCell()
		:GridCell(XMFLOAT3{ -1.f,-1.f,-1.f }, XMFLOAT3{ -1.f,-1.f,-1.f })
		{
			isValid = false;
		}
};

class GridComponent : public BaseComponent
{
public:
	explicit GridComponent(std::vector<char>& gridMap, XMFLOAT3 bottomLeft, XMFLOAT3 topRight, float cellSize, float scaleFactor);
	~GridComponent() override = default;

	GridComponent(const GridComponent& other) = delete;
	GridComponent(GridComponent&& other) noexcept = delete;
	GridComponent& operator=(const GridComponent& other) = delete;
	GridComponent& operator=(GridComponent&& other) noexcept = delete;

	void MoveObject(GameObject* pObject, int row, int col);
	void MoveObject(GameObject* pObject, GridCell& newGridCell);

	void PlaceObject(GameObject* pObject, GridCell& cell);
	void PlaceObject(GameObject* pObject, int row, int col);

	void Explode(GridCell& cell, bool allowDropsFromStones = true);
	void Explode(int row, int col, bool allowDropsFromStones = true);

	GridCell& GetCell(const GameObject& gameObject);
	GridCell& GetCell(const XMFLOAT3& position);
	//Assume that the bottom left one is [1,1]
	GridCell& GetCell(int row, int col);
	GridCell& GetCell(int index) { return m_GridCells[index]; }

	int GetCellIndex(const GridCell& cell) const;

	//Make sure to check if the cell is valid before using it.
	GridCell& GetCellToTheRight(const GridCell& cell);
	GridCell& GetCellToTheLeft(const GridCell& cell);
	GridCell& GetCellOnTop(const GridCell& cell);
	GridCell& GetCellUnder(const GridCell& cell);

	GridCell& GetCellToTheRight(int row, int col) { return GetCellToTheRight(GetCell(row, col)); }
	GridCell& GetCellToTheLeft(int row, int col) { return GetCellToTheLeft(GetCell(row, col)); }
	GridCell& GetCellOnTop(int row, int col) { return GetCellOnTop(GetCell(row, col)); }
	GridCell& GetCellUnder(int row, int col) { return GetCellUnder(GetCell(row, col)); }

	float GetCellSize() const { return m_CellSize / 2.f; }
	void UpdateCharacterOnMap(std::vector<BombermanCharacter*>& players);

	float GetScaleFactor() const { return m_CellScaleFactor * 30; }

	void ClearGrid();

protected:
	void Initialize(const SceneContext&) override{};
	void Update(const SceneContext&) override{};
	void Draw(const SceneContext& sceneContext) override;

private:
	std::vector<char>& m_GridMap;
	std::vector<GridCell> m_GridCells{};
	std::vector<GameObject*> m_pObjectsInGrid{};

	std::unordered_map<BombermanCharacter*, int> m_PrevCharPositions{};

	XMFLOAT3 m_BottomLeft{}, m_TopRight{};

	int m_NumOfRows{}, m_NumOfColumns{};
	InvalidCell m_InvalidCell{};

	float m_CellSize{};
	float m_CellScaleFactor{};

	void InitializeCells();
	char GetCharOfObject(GameObject* const gameObject) const;
};
