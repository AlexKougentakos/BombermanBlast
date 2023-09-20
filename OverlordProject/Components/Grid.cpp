#include "stdafx.h"
#include "Grid.h"

#include "PowerUpManager.h"
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/PowerUps/PowerUp.h"

#include "Components/GameObjectManager.h"

GridComponent::GridComponent(std::vector<char>& gridMap, XMFLOAT3 bottomLeft, XMFLOAT3 topRight, float cellSize, float scaleFactor)
	:m_GridMap(gridMap),
	m_BottomLeft(bottomLeft),
	m_TopRight(topRight),
	m_CellSize(cellSize),
    m_CellScaleFactor(scaleFactor)
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

void GridComponent::Draw(const SceneContext& sceneContext)
{
    if (sceneContext.settings.drawPhysXDebug)
    {

        const auto drawCol = XMFLOAT4{ Colors::BlueViolet };

        for (const auto& cell : m_GridCells)
        {
            DebugRenderer::DrawLine(cell.bottomLeft, { cell.bottomLeft.x, cell.bottomLeft.y, cell.topRight.z }, drawCol);
            DebugRenderer::DrawLine({ cell.bottomLeft.x, cell.bottomLeft.y, cell.topRight.z }, cell.topRight, drawCol);
            DebugRenderer::DrawLine(cell.topRight, { cell.topRight.x, cell.bottomLeft.y, cell.bottomLeft.z }, drawCol);
            DebugRenderer::DrawLine({ cell.topRight.x, cell.bottomLeft.y, cell.bottomLeft.z }, cell.bottomLeft, drawCol);
        }
    }
}

//Todo: Rework or remove this function if possible
char GridComponent::GetObjectOnCell(const GridCell& cell) const
{
    for (const auto cellToCompare : m_GridCells)
    {
	    if (cellToCompare.center.x == cell.center.x &&
            cellToCompare.center.z == cell.center.y)
	    {
            return GetCharOfObject(cell.objects[0]);
	    }
    }

    return 'O';
}

void GridComponent::PlaceObject(GameObject* pObject, int row, int col)
{
    PlaceObject(pObject, GetCell(row, col));
}

void GridComponent::RemoveButKeepAlive(GameObject* pObject)
{
	const int index{GetCellIndex(GetCell(*pObject))};
    m_GridCells[index].Remove(pObject);

    pObject->GetTransform()->Translate(0, -100, 0);
}


void GridComponent::PlaceObject(GameObject* pObject, GridCell& cell) 
{
    if (!pObject) return;

    const std::wstring objectTag = pObject->GetTag();

    if (!pObject->GetScene())
    	GetScene()->AddChild(pObject);

    m_GridCells[GetCellIndex(cell)].objects.emplace_back(pObject);

    float yOffset{0.f};
    if (objectTag == L"PowerUp")
		yOffset = 4.5f;

    if (objectTag == L"Player")
		yOffset = 10.f;

    pObject->GetTransform()->Translate(XMFLOAT3{ cell.center.x, yOffset, cell.center.z });
}


void GridComponent::DeleteSpecificObject(GameObject* pObject)
{

    for (auto& cell : m_GridCells)
        for (auto toCompare = cell.objects.begin(); toCompare != cell.objects.end();)
        {
            // Check if the current object matches the one to be removed
            if (*toCompare == pObject)
            {
                GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(pObject);
                toCompare = cell.objects.erase(toCompare);
            }
            else ++toCompare;
        }
}

void GridComponent::ClearGrid()
{
    for (auto& cell : m_GridCells)
        for (const auto& object : cell.objects)
        {
            if (object->GetTag() == L"Rock" ||
                object->GetTag() == L"Bomb" ||
                object->GetTag() == L"PowerUp")
                DeleteSpecificObject(object);
        }
}


void GridComponent::TryToRemoveAllObjects(int row, int col)
{
    TryToRemoveAllObjects(GetCell(row, col));
}

void GridComponent::TryToRemoveAllObjects(GridCell& cell)
{
	auto& vec = m_GridCells[GetCellIndex(cell)].objects;
    std::vector<GameObject*> objectsToKeep{}; //Hot fix to prevent the player from being removed. Should
                                            // be replaced with a proper handle for the player death.

    std::vector<GridCell> cellsForPowerUpSpawn{};

    for (const auto object : vec)
    {
        if (object->GetTag() == L"Bomb")
        {
            objectsToKeep.emplace_back(object);
            continue;
        }

	    if (object->GetTag() == L"Player")
	    {
            dynamic_cast<BombermanCharacter*>(object)->Kill();
            objectsToKeep.emplace_back(object);
            continue;
	    }

        if (object->GetTag() == L"Rock")
        {
            //don't add when itterating over
            cellsForPowerUpSpawn.emplace_back(cell);
        }

        if (object->GetTag() == L"PowerUp")
        {
            objectsToKeep.emplace_back(object);
			continue;
		}

        //TODO: Implement explode function on all game objects
        //GetScene()->RemoveChild(object, true);
        GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(object);
    }

    vec = std::move(objectsToKeep);

    for (auto& cellForPowerUp : cellsForPowerUpSpawn)
    {
        GetGameObject()->GetComponent<PowerUpManager>()->TriggerPowerUpSpawn(cellForPowerUp);
	}
}

void GridComponent::MoveObject(GameObject* pObject, int row, int col)
{
		MoveObject(pObject, GetCell(row, col));
}

void GridComponent::MoveObject(GameObject* pObject, GridCell& newGridCell)
{
    for (auto& cell : m_GridCells)
    {
        auto& objects = cell.objects;

        auto it = std::find(objects.begin(), objects.end(), pObject);
        if (it != objects.end())
        {
            objects.erase(it);

            const float yOffset{ pObject->GetTag() == L"Player" ? 10.f : 0.f };

            newGridCell.objects.emplace_back(pObject);
            pObject->GetTransform()->Translate(XMFLOAT3{ newGridCell.center.x,yOffset, newGridCell.center.z });

            return;
        }
    }

	// If we reach this point, the object wasn't found in any cell
    std::cout << "The object wasn't found in any cell!" << std::endl;
}


char GridComponent::GetCharOfObject(const GameObject* const gameObject) const
{
    if (gameObject->GetTag() == L"Player")
        return 'P';
    if (gameObject->GetTag() == L"Explosion")
        return 'X';
    if (gameObject->GetTag() == L"Enemy")
		return 'E';
    if (gameObject->GetTag() == L"Bomb")
        return 'B';
    if (gameObject->GetTag() == L"Rock")
		return 'R';
    if (gameObject->GetTag() == L"PowerUp")
        return 'U';

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
    if (index >= m_GridCells.size())
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
    m_NumOfRows = static_cast<int>((m_TopRight.z - m_BottomLeft.z) / m_CellSize) + 1;
    //todo: fix the rows/ columns issue

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

void GridComponent::UpdateCharacterOnMap(std::vector<BombermanCharacter*>& players)
{
    //Update the cells to contain the right character

    for (BombermanCharacter* character : players)
    {
        /*if (character->IsDead())
        {
	        std::erase(players, character);
            DeleteSpecificObject(character);
			continue;
        }*/

        const XMFLOAT3 characterPos = character->GetTransform()->GetPosition();
        const int characterCellIdx = GetCellIndex(GetCell(characterPos));
        //If the cell doesn't contain the character add it
        if (!GetCell(characterPos).Contains(character))
        {
            m_GridCells[characterCellIdx].objects.emplace_back(character);
        }

        // Iterate through all cells to remove characters from incorrect cells
        for (size_t currentCellIdx = 0; currentCellIdx < m_GridCells.size(); ++currentCellIdx)
        {
            if (currentCellIdx != characterCellIdx)
            {
                GridCell& currentCell = m_GridCells[currentCellIdx];
                currentCell.Remove(character);
            }
        }
    }

    for (size_t currentCellIdx{ 0 }; currentCellIdx < m_GridCells.size(); ++currentCellIdx)
    {
        GridCell currentCell = m_GridCells[currentCellIdx];
        if (currentCell.objects.empty())
        {
            m_GridMap[currentCellIdx] = 'O';
            continue;
        }

        //Update the map (draw the first element on there)
        m_GridMap[currentCellIdx] = GetCharOfObject(currentCell.objects[0]);

    }
}