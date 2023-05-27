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
GridCell& GridComponent::GetCell(const GameObject& gameObject)
{
    const XMFLOAT3 position = gameObject.GetTransform()->GetPosition();
    return GetCell(position);
}
GridCell& GridComponent::GetCell(const XMFLOAT3& position)
{
    for (auto& cell : m_GridCells)
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


GridCell& GridComponent::GetCell(int row, int col)
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

char GridComponent::GetObjectOnCell(const GridCell& cell)
{
    for (const auto object : m_pObjectsInGrid)
    {
        const GridCell toCompare = GetCell(*object);
		if (toCompare.center.x == cell.center.x && toCompare.center.z == cell.center.z)
			return GetCharOfObject(object);
	}
    return 'O';
}

void GridComponent::PlaceObject(GameObject* pObject, int row, int col)
{
    PlaceObject(pObject, GetCell(row, col));
}

void GridComponent::PlaceObject(GameObject* pObject, GridCell& cell) 
{
	if (m_GridMap[GetCellIndex(cell)] == 'O' ||
        //If the cell is occupied by a player, we can still place a bomb on it
        (m_GridMap[GetCellIndex(cell)] == 'P' && GetCharOfObject(pObject) == 'B'))
	{
        cell.objects.emplace_back(pObject);

        m_pObjectsInGrid.emplace_back(GetScene()->AddChild(pObject));
        pObject->GetTransform()->Translate(XMFLOAT3{ cell.center.x, 0.f, cell.center.z });
		m_GridMap[GetCellIndex(cell)] = GetCharOfObject(pObject);
	}
}

void GridComponent::RemoveObject(int row, int col)
{
    RemoveObject(GetCell(row, col));
}

void GridComponent::RemoveObject(GridCell& cell)
{
    for (auto it = m_pObjectsInGrid.begin(); it != m_pObjectsInGrid.end();)
    {
        const auto& object = *it;
        const GridCell toCompare = GetCell(*object);

        if (toCompare.center.x == cell.center.x && toCompare.center.z == cell.center.z)
        {
            if (object->GetTag() == L"Player")
            {
                // Call player death function here
                Logger::LogTodo(L"Add Player Destruction");
                return;
            }

            //Update the cells directly
            auto& vec = m_GridCells[GetCellIndex(cell)].objects;
            vec.erase(std::remove(vec.begin(), vec.end(), object), vec.end());

            GetScene()->RemoveChild(object, true);
            m_GridMap[GetCellIndex(cell)] = 'O';

            // Erase the removed object from m_pObjectsInGrid and update the iterator
            it = m_pObjectsInGrid.erase(it);
        }
        else
        {
            // Move to the next object
            ++it;
        }
    }

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
GridCell& GridComponent::GetCellUnder(const GridCell& cell)
{
    const int index = GetCellIndex(cell) - m_NumOfColumns;
    if (index < 0)
        return m_InvalidCell;


	GridCell& cellToReturn = m_GridCells[index];
    if (cellToReturn.center.x == cell.center.x)
        return cellToReturn;

    return m_InvalidCell;
}

GridCell& GridComponent::GetCellOnTop(const GridCell& cell)
{
    const int index = GetCellIndex(cell) + m_NumOfColumns;
    if (index > m_GridCells.size())
        return m_InvalidCell;

    GridCell& cellToReturn = m_GridCells[index];
    if (cellToReturn.center.x == cell.center.x)
        return cellToReturn;

    return m_InvalidCell;
}

 GridCell& GridComponent::GetCellToTheLeft(const GridCell& cell)
{
    const int index = GetCellIndex(cell) - 1;
    if (index < 0)
        return m_InvalidCell;

    GridCell& cellToReturn = m_GridCells[index];

    if (cellToReturn.center.z == cell.center.z)
        return cellToReturn;

    return m_InvalidCell;
}

GridCell& GridComponent::GetCellToTheRight(const GridCell& cell)
{
    const int index = GetCellIndex(cell) + 1;
    if (index > m_GridCells.size())
        return m_InvalidCell;

    GridCell& cellToReturn = m_GridCells[index];

    if (cellToReturn.center.z == cell.center.z)
        return cellToReturn;

    return m_InvalidCell;
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

void GridComponent::UpdateCharacterOnMap(XMFLOAT3 position)
{
#ifdef _DEBUG
    //Override the cell and draw the player
    const GridCell playerCell = GetCell(position);
    const int index = GetCellIndex(playerCell);
    if (m_GridMap[index] == 'O')
        m_GridMap[index] = 'P';

    for (size_t currentCell{ 0 }; currentCell < m_GridCells.size(); ++currentCell)
    {
        const auto toCompare = m_GridCells[currentCell];
        if ((toCompare.center.x != playerCell.center.x ||
            toCompare.center.z != playerCell.center.z)
            && m_GridMap[currentCell] == 'P')
        {
            m_GridMap[currentCell] = 'O';
        }
    }

    for (size_t currentCellIdx{ 0 }; currentCellIdx < m_GridCells.size(); ++currentCellIdx)
    {
        GridCell currentCell = m_GridCells[currentCellIdx];
        if (currentCell.objects.empty()) return;

        //Update the map (draw the first element on there)
        m_GridMap[currentCellIdx] = GetCharOfObject(currentCell.objects[0]);
    }
#endif
}
