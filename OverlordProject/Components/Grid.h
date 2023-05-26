#pragma once

class MeshFilter;
class ModelAnimator;

struct GridCell
{
	GridCell(XMFLOAT3 bottomLeft, XMFLOAT3 topRight)
		:bottomLeft(bottomLeft),
		topRight(topRight)
	{
		center = XMFLOAT3((bottomLeft.x + topRight.x) / 2.f, bottomLeft.y, (bottomLeft.z + topRight.z) / 2.f);
	}

	XMFLOAT3 bottomLeft{};
	XMFLOAT3 topRight{};
	XMFLOAT3 center{};

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
	explicit GridComponent(std::vector<char>& gridMap, XMFLOAT3 bottomLeft, XMFLOAT3 topRight, float cellSize);
	~GridComponent() override = default;

	GridComponent(const GridComponent& other) = delete;
	GridComponent(GridComponent&& other) noexcept = delete;
	GridComponent& operator=(const GridComponent& other) = delete;
	GridComponent& operator=(GridComponent&& other) noexcept = delete;

	void PlaceObject(GameObject* pObject, const GridCell& cell);
	void PlaceObject(GameObject* pObject, int row, int col);
	void RemoveObject( const GridCell& cell) const;
	void RemoveObject( int row, int col) const;

	const GridCell& GetCell(const GameObject& gameObject) const;
	const GridCell& GetCell(const XMFLOAT3& position) const;
	//Assume that the bottom left one is [1,1]
	const GridCell& GetCell(int row, int col) const;
	const GridCell& GetCell(int index) const { return m_GridCells[index]; }

	int GetCellIndex(const GridCell& cell) const;

	//Make sure to check if the cell is valid before using it.
	const GridCell& GetCellToTheRight(const GridCell& cell) const;
	const GridCell& GetCellToTheLeft(const GridCell& cell) const;
	const GridCell& GetCellOnTop(const GridCell& cell) const;
	const GridCell& GetCellUnder(const GridCell& cell) const;

	const GridCell& GetCellToTheRight(int row, int col) const { return GetCellToTheRight(GetCell(row, col)); }
	const GridCell& GetCellToTheLeft(int row, int col) const { return GetCellToTheLeft(GetCell(row, col)); }
	const GridCell& GetCellOnTop(int row, int col) const { return GetCellOnTop(GetCell(row, col)); }
	const GridCell& GetCellUnder(int row, int col) const { return GetCellUnder(GetCell(row, col)); }

protected:
	void Initialize(const SceneContext&) override{};
	void Update(const SceneContext&) override{};
	void Draw(const SceneContext& sceneContext) override;

private:
	std::vector<char>& m_GridMap;
	std::vector<GridCell> m_GridCells{};
	std::vector<GameObject*> m_pObjectsInGrid{};

	XMFLOAT3 m_BottomLeft{}, m_TopRight{};

	int m_NumOfRows{}, m_NumOfColumns{};

	float m_CellSize{};

	void InitializeCells();
	char GetCharOfObject(const GameObject* const gameObject) const;
};
