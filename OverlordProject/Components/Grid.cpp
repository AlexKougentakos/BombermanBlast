#include "stdafx.h"
#include "Grid.h"

#include <unordered_set>

#include "PowerUpManager.h"
#include "Prefabs/BombermanCharacter.h"
#include "Prefabs/PowerUps/PowerUp.h"
#include "Helpers/ExplosionHandler.h"
#include "Prefabs/RockPrefab.h"

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
    for (GridCell& cell : m_GridCells)
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

void GridComponent::PlaceObject(GameObject* pObject, int row, int col)
{
    PlaceObject(pObject, GetCell(row, col));
}

void GridComponent::PlaceObject(GameObject* pObject, GridCell& cell) 
{
    if (!pObject) return;

    const std::wstring objectTag = pObject->GetTag();

    if (!pObject->GetScene())
    	GetScene()->AddChild(pObject);

    //Explosions don't have to be kept track of, they will disappear, only their position is needed.
    if (objectTag != L"Explosion" && objectTag != L"PowerUp")
        m_GridCells[GetCellIndex(cell)].objects.emplace_back(pObject);

    float yOffset{0.f};
    if (objectTag == L"PowerUp")
		yOffset = 4.5f;

    if (objectTag == L"Player")
		yOffset = 10.f;

    pObject->GetTransform()->Translate(XMFLOAT3{ cell.center.x, yOffset, cell.center.z });
}

void GridComponent::ClearGrid()
{
    for (auto& cell : m_GridCells)
        for (const auto& object : cell.objects)
        {
            if (object->GetTag() == L"Rock" ||
                object->GetTag() == L"Bomb" ||
                object->GetTag() == L"PowerUp")
                GetGameObject()->GetComponent<GameObjectManager>()->RemoveGameObject(object);
        }
}


void GridComponent::Explode(int row, int col)
{
    Explode(GetCell(row, col));
}

void GridComponent::Explode(GridCell& cell)
{
	auto& vec = m_GridCells[GetCellIndex(cell)].objects;
    std::vector<GameObject*> objectsToKeep{}; //Hot fix to prevent the player from being removed. Should
                                            // be replaced with a proper handle for the player death.

    std::vector<GridCell> cellsForPowerUpSpawn{};

    ExplosionHandler explosionHandler{};

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
            //don't add when iterating over
            cellsForPowerUpSpawn.emplace_back(cell);
            explosionHandler.Visit(dynamic_cast<RockPrefab*>(object), &cell);
        }

        if (object->GetTag() == L"PowerUp")
        {
            objectsToKeep.emplace_back(object);
            explosionHandler.Visit(dynamic_cast<BasePowerUp*>(object), &cell);
			continue;
		}

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
    if (index >= m_GridCells.size())
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
    // Use a set to check which characters are still in the game this frame.
    std::unordered_set<BombermanCharacter*> currentPlayers(players.begin(), players.end());

    // Remove characters that are no longer in the game.
    for (auto it = m_PrevCharPositions.begin(); it != m_PrevCharPositions.end(); )
    {
        if (currentPlayers.find(it->first) == currentPlayers.end())
        {
            // Remove the character from its last known cell
            m_GridCells[it->second].Remove(it->first);
            it = m_PrevCharPositions.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Update the cells to contain the right character.
    for (BombermanCharacter* character : players)
    {
        if (!character) continue;

        const XMFLOAT3 characterPos = character->GetTransform()->GetPosition();
        const int characterCellIdx = GetCellIndex(GetCell(characterPos));

        // If the character has moved, remove it from its old cell.
        if (m_PrevCharPositions.find(character) != m_PrevCharPositions.end() && m_PrevCharPositions[character] != characterCellIdx)
        {
            m_GridCells[m_PrevCharPositions[character]].Remove(character);
        }
        m_PrevCharPositions[character] = characterCellIdx;

        // If the cell doesn't contain the character add it.
        if (!GetCell(characterPos).Contains(character))
        {
            m_GridCells[characterCellIdx].objects.emplace_back(character);
        }
    }

    // Update the visual map representation.
    for (size_t currentCellIdx{ 0 }; currentCellIdx < m_GridCells.size(); ++currentCellIdx)
    {
        GridCell currentCell = m_GridCells[currentCellIdx];
        if (currentCell.objects.empty())
        {
            m_GridMap[currentCellIdx] = 'O';
            continue;
        }

        // Update the map (draw the first element on there).
        m_GridMap[currentCellIdx] = GetCharOfObject(currentCell.objects[0]);
    }
}