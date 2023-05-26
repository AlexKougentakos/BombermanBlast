#include "stdafx.h"
#include "Grid.h"

#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/BombPrefab.h"

GridComponent::GridComponent(std::vector<char>& gridMap, XMFLOAT3 bottomLeft, XMFLOAT3 topRight, float cellSize)
	:m_GridMap(gridMap),
	m_BottomLeft(bottomLeft),
	m_TopRight(topRight),
	m_CellSize(cellSize)
{
	InitializeCells();
}

const GridCell& GridComponent::GetCell(const GameObject& gameObject) const
{
    const XMFLOAT3 position = gameObject.GetTransform()->GetPosition();
    return GetCell(position);
}

const GridCell& GridComponent::GetCell(const XMFLOAT3& position) const
{
    for (const auto& cell : m_GridCells)
    {
	    if (position.x >= cell.bottomLeft.x && position.x <= cell.topRight.x &&
                    	position.z >= cell.bottomLeft.z && position.z <= cell.topRight.z)
	    {
	    	return cell;
		}
	}

    assert(false && "You selected an invalid cell");

    //We assume it will never get here
    return m_GridCells[0];
}

int GridComponent::GetCellIndex(const GridCell& cell) const
{
	for (auto it = m_GridCells.begin(); it != m_GridCells.end(); ++it)
	{
		if (it->center.x == cell.center.x &&
            it->center.y == cell.center.y &&
            it->center.z == cell.center.z)
		{
			return static_cast<int>(std::distance(m_GridCells.begin(), it));
		}
	}

    return -1;
}


const GridCell& GridComponent::GetCell(int row, int col) const
{
    row -= 1;
    col -= 1;
    const int numOfRows = static_cast<int>((m_TopRight.x - m_BottomLeft.x) / m_CellSize);

    return m_GridCells[row * numOfRows + col];
}

void GridComponent::Draw(const SceneContext& /*sceneContext*/)
{
	const auto drawCol = XMFLOAT4{ Colors::BlueViolet };

	for (const auto& cell : m_GridCells)
	{
		DebugRenderer::DrawLine(cell.bottomLeft, {cell.bottomLeft.x, cell.bottomLeft.y, cell.topRight.z},drawCol);
		DebugRenderer::DrawLine({ cell.bottomLeft.x, cell.bottomLeft.y, cell.topRight.z }, cell.topRight,drawCol);
		DebugRenderer::DrawLine(cell.topRight, { cell.topRight.x, cell.bottomLeft.y, cell.bottomLeft.z}, drawCol);
		DebugRenderer::DrawLine({ cell.topRight.x, cell.bottomLeft.y, cell.bottomLeft.z }, cell.bottomLeft, drawCol);
	}
}

void GridComponent::PlaceObject(GameObject* pObject, int row, int col)
{
    PlaceObject(pObject, GetCell(row, col));
}

void GridComponent::PlaceObject(GameObject* pObject, const GridCell& cell) 
{
	if (m_GridMap[GetCellIndex(cell)] == 'O')
	{
        m_pObjectsInGrid.emplace_back(GetScene()->AddChild(pObject));
        pObject->GetTransform()->Translate(XMFLOAT3{ cell.center.x, 0.f, cell.center.z });
		m_GridMap[GetCellIndex(cell)] = GetCharOfObject(pObject);
	}
}

void GridComponent::RemoveObject(int row, int col) const
{
    RemoveObject(GetCell(row, col));
}

void GridComponent::RemoveObject(const GridCell& cell) const
{
	for (const auto object : m_pObjectsInGrid)
	{
        const GridCell toCompare = GetCell(*object);
		if (toCompare.center.x == cell.center.x &&
            toCompare.center.z == cell.center.z)
		{
            if (object->GetTag() == L"Player")
            {
            	//todo: call player death function
                Logger::LogTodo(L"Add player destroy function");
                return;
            }

            GetScene()->RemoveChild(object, true);
            m_GridMap[GetCellIndex(cell)] = 'O';
            return;
		}
	}


    m_GridMap[GetCellIndex(cell)] = 'O';
}

char GridComponent::GetCharOfObject(const GameObject* const gameObject) const
{
    if (gameObject->GetTag() == L"Enemy")
		return 'E';
    if (gameObject->GetTag() == L"Bomb")
        return 'B';
    if (gameObject->GetTag() == L"Rock")
		return 'R';

    return 'O';
}

const GridCell& GridComponent::GetCellUnder(const GridCell& cell) const
{
    return m_GridCells[GetCellIndex(cell) - m_NumOfColumns];
}

const GridCell& GridComponent::GetCellOnTop(const GridCell& cell) const
{
    return m_GridCells[GetCellIndex(cell) + m_NumOfColumns];
}

const GridCell& GridComponent::GetCellToTheLeft(const GridCell& cell) const
{
    return m_GridCells[GetCellIndex(cell) - 1];
}

const GridCell& GridComponent::GetCellToTheRight(const GridCell& cell) const
{
    return m_GridCells[GetCellIndex(cell) + 1];
}

void GridComponent::InitializeCells()
{
    // Calculate the number of cells in x and y directions
    m_NumOfColumns = static_cast<int>((m_TopRight.x - m_BottomLeft.x) / m_CellSize);
    m_NumOfRows = static_cast<int>((m_TopRight.z - m_BottomLeft.z) / m_CellSize);

    m_GridCells.reserve(m_NumOfColumns * m_NumOfRows);

    // Iterate over the grid in x and y directions
	for (int z = 0; z < m_NumOfRows; ++z)
    {
        for (int x = 0; x < m_NumOfColumns; ++x)
        {
            // Calculate the bottom-left and top-right corners of the current cell
            XMFLOAT3 cellBottomLeft(m_BottomLeft.x + x * m_CellSize,
                m_BottomLeft.y + 0.7f ,
                m_BottomLeft.z + z * m_CellSize);

            XMFLOAT3 cellTopRight(cellBottomLeft.x + m_CellSize,
                cellBottomLeft.y,
                cellBottomLeft.z + m_CellSize);

            // Create a new cell and add it to the grid
            m_GridCells.emplace_back(cellBottomLeft, cellTopRight);
        }
    }
}