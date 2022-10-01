#include "chunk.h"

int Chunk::getBlocksIndex(int x, int y, int z) {
	return x * CHUNK_SIZE.x * CHUNK_SIZE.x + y * CHUNK_SIZE.y + z;
}

Chunk::Chunk() {
	
}

Chunk::Chunk (ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT3 position)
{
	blocks = new Block[(int)CHUNK_SIZE.x * (int)CHUNK_SIZE.y * (int)CHUNK_SIZE.z];
	std::vector<Vertex> chunkVertices = std::vector<Vertex>();
	XMFLOAT3 blockColour = { 0.0f, 1.0f, 0.0f };

	// Send all the blocks to the mesh...
	for (int x=0; x < CHUNK_SIZE.x; x++) {
		for (int y = 0; y < CHUNK_SIZE.y; y++) {
			for (int z = 0; z < CHUNK_SIZE.z; z++) {
				Block block{};

				if (x % 2 == 0 && y % 2 == 0) {
					block.colour = blockColour;
				}
				else {
					block.colour = { 1.0f, 0.0f, 0.0f };
				}

				block.isAir = false;
				block.isVisible = true;
				block.visibleBlockFaces = std::vector<BlockFaces>();

				blocks[getBlocksIndex(x, y, z)] = block;
			}
		}
	}

	setVisibleBlocks();
	chunkVertices = setVerticesByPosition();

	mesh = Mesh(
		device,
		chunkVertices.data(),
		chunkVertices.size(),
		position,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f }
	);
}

void Chunk::update(ID3D11DeviceContext* context, XMFLOAT4X4 view, XMFLOAT4X4 projection) {
	mesh.update(context, view, projection);
}

void Chunk::draw(ID3D11DeviceContext* context) {
	mesh.draw(context);
}

void Chunk::setVisibleBlocks() {
	int numVisible = 0;

	for (int x = 0; x < CHUNK_SIZE.x; x++) {
		for (int y = 0; y < CHUNK_SIZE.y; y++) {
			for (int z = 0; z < CHUNK_SIZE.z; z++) {
				if (!blocks[getBlocksIndex(x, y, z)].isAir) {

					Block& block = blocks[getBlocksIndex(x, y, z)];
					block.visibleBlockFaces.clear();

					// Loop through adjacent blocks...
					bool shouldBeVisible = false;

					if (x-1 > 0 && x-1 < 15) // Left
					{
						if (blocks[getBlocksIndex(x-1, y, z)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::LEFT);
						}
					}

					if (x + 1 > 0 && x + 1 < 15) // Right
					{
						if (blocks[getBlocksIndex(x + 1, y, z)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::RIGHT);
						}
					}

					if (y - 1 > 0 && y - 1 < 15) // Bottom
					{
						if (blocks[getBlocksIndex(x, y-1, z)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::BOTTOM);
						}
					}

					if (y + 1 > 0 && y + 1 < 15) // Top
					{
						if (blocks[getBlocksIndex(x, y+1, z)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::TOP);
						}
					}

					if (z - 1 > 0 && z - 1 < 15) // Back
					{
						if (blocks[getBlocksIndex(x, y, z-1)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::BACK);
						}
					}

					if (z + 1 > 0 && z + 1 < 15) // Front
					{
						if (blocks[getBlocksIndex(x, y, z+1)].isAir) {
							shouldBeVisible = true;
							block.visibleBlockFaces.push_back(BlockFaces::FRONT);
						}
					}

					if (x == 0)
					{
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::LEFT);
					}
					if (x == CHUNK_SIZE.x-1) {
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::RIGHT);
					}
					if (y == 0) {
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::BOTTOM);
					}

					if (y == CHUNK_SIZE.y-1) {
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::TOP);
					}

					if (z == 0) {
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::BACK);
					}

					if (z == CHUNK_SIZE.z - 1) {
						shouldBeVisible = true;
						block.visibleBlockFaces.push_back(BlockFaces::FRONT);
					}

					blocks[getBlocksIndex(x, y, z)].isVisible = shouldBeVisible;

					if (shouldBeVisible)
					{
						numVisible++;
					}
				}
			}
		}
	}

	assert(numVisible > 0);
}

std::vector<Vertex> Chunk::setVerticesByPosition() {

	std::vector<Vertex> chunkVertices = std::vector<Vertex>();
	float blockSize = VOXEL_SIZE;

	for (int x = 0; x < CHUNK_SIZE.x; x++) {
		for (int y = 0; y < CHUNK_SIZE.y; y++) {
			for (int z = 0; z < CHUNK_SIZE.z; z++) {

				Block& currentBlock = blocks[getBlocksIndex(x, y, z)];

				if (!currentBlock.isAir && currentBlock.isVisible) {
					float xPos = (x * blockSize) + (blockSize / 2);
					float xNeg = (x * blockSize) - (blockSize / 2);
					float yPos = (y * blockSize) + (blockSize / 2);
					float yNeg = (y * blockSize) - (blockSize / 2);
					float zPos = (z * blockSize) + (blockSize / 2);
					float zNeg = (z * blockSize) - (blockSize / 2);

					for (const BlockFaces& face : currentBlock.visibleBlockFaces) {
						switch (face) {
						case BlockFaces::LEFT:
						{
							/* -X */
							chunkVertices.push_back({
							XMFLOAT3(xNeg,yNeg,zNeg), currentBlock.colour, // 0
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos,zNeg), currentBlock.colour, // 2
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg,yNeg, zPos), currentBlock.colour,// 1
								});

							chunkVertices.push_back({
								XMFLOAT3(xNeg,yNeg, zPos), currentBlock.colour,// 1
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos,zNeg), currentBlock.colour, // 2
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos, zPos), currentBlock.colour, // 3
								});
						}
						break;
						case BlockFaces::RIGHT:
						{
							/* +X */
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg,zNeg), currentBlock.colour, // 4
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg, zPos), currentBlock.colour, // 5
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos,zNeg), currentBlock.colour, // 6
								});

							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg, zPos), currentBlock.colour, // 5
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos, zPos), currentBlock.colour, // 7
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos,zNeg), currentBlock.colour, // 6
								});
						}
						break;
						case BlockFaces::BOTTOM:
						{
							/* -Y */
							chunkVertices.push_back({
							XMFLOAT3(xNeg,yNeg,zNeg), currentBlock.colour, // 0
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg,yNeg, zPos), currentBlock.colour,// 1
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg, zPos), currentBlock.colour, // 5
								});

							chunkVertices.push_back({
							XMFLOAT3(xNeg,yNeg,zNeg), currentBlock.colour, // 0
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg, zPos), currentBlock.colour, // 5
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg,zNeg), currentBlock.colour, // 4
								});
						}
						break;
						case BlockFaces::TOP:
						{
							/* +Y */
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos,zNeg), currentBlock.colour, // 2
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos,zNeg), currentBlock.colour, // 6
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos, zPos), currentBlock.colour, // 7
								});

							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos,zNeg), currentBlock.colour, // 2
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos, zPos), currentBlock.colour, // 7
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos, zPos), currentBlock.colour, // 3
								});
						}
						break;
						case BlockFaces::BACK:
						{
							/* -Z */
							chunkVertices.push_back({
							XMFLOAT3(xNeg,yNeg,zNeg), currentBlock.colour, // 0
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg,zNeg), currentBlock.colour, // 4
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos,zNeg), currentBlock.colour, // 6
								});

							chunkVertices.push_back({
							XMFLOAT3(xNeg,yNeg,zNeg), currentBlock.colour, // 0
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos,zNeg), currentBlock.colour, // 6
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos,zNeg), currentBlock.colour, // 2
								});
						}
						break;
						case BlockFaces::FRONT:
						{
							/* +Z */
							chunkVertices.push_back({
								XMFLOAT3(xNeg,yNeg, zPos), currentBlock.colour,// 1
								});
							chunkVertices.push_back({
								XMFLOAT3(xNeg, yPos, zPos), currentBlock.colour, // 3
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos, zPos), currentBlock.colour, // 7
								});

							chunkVertices.push_back({
								XMFLOAT3(xNeg,yNeg, zPos), currentBlock.colour,// 1
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos, yPos, zPos), currentBlock.colour, // 7
								});
							chunkVertices.push_back({
								XMFLOAT3(xPos,yNeg, zPos), currentBlock.colour // 5
								});
						}
						break;
						}
					}
				}
			}
		}
	}

	return chunkVertices;
}

void Chunk::buildMesh(ID3D11DeviceContext* context) {
	XMFLOAT3 blockColour = { 0.0f, 1.0f, 0.0f };

	// Send all the blocks to the mesh...
	for (int x = 0; x < CHUNK_SIZE.x; x++) {
		for (int y = 0; y < CHUNK_SIZE.y; y++) {
			for (int z = 0; z < CHUNK_SIZE.z; z++) {
				Block block{};

				if (x % 2 == 0 && y % 2 == 0) {
					block.colour = blockColour;
				}
				else {
					block.colour = { 1.0f, 0.0f, 0.0f };
				}

				block.isAir = false;
				block.isVisible = true;
				block.visibleBlockFaces = std::vector<BlockFaces>();

				blocks[getBlocksIndex(x, y, z)] = block;
			}
		}
	}

	setVisibleBlocks();
	mesh.setPoints(setVerticesByPosition(), context);
}