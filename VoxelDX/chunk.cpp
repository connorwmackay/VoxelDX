#include "chunk.h"

Chunk::Chunk() {
	
}

Chunk::Chunk (ID3D11Device* device, XMFLOAT3 position)
{
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

				blocks[x][y][z] = block;
			}
		}
	}

	setVisibleBlocks();

	// Build the Mesh
	std::vector<Vertex> chunkVertices = std::vector<Vertex>();

		for (int x = 0; x < CHUNK_SIZE.x; x++) {
			for (int y = 0; y < CHUNK_SIZE.y; y++) {
			for (int z = 0; z < CHUNK_SIZE.z; z++) {
				if (!blocks[x][y][z].isAir && blocks[x][y][z].isVisible) {
					float xPos = x + 0.5f;
					float xNeg = x - 0.5f;
					float yPos = y + 0.5f;
					float yNeg = y - 0.5f;
					float zPos = z + 0.5f;
					float zNeg = z - 0.5f;

					/* -X */
					chunkVertices.push_back({
					XMFLOAT3(xNeg,yNeg,zNeg), blocks[x][y][z].colour, // 0
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos,zNeg), blocks[x][y][z].colour, // 2
						});
					chunkVertices.push_back({
						XMFLOAT3(xNeg,yNeg, zPos), blocks[x][y][z].colour,// 1
					});

					chunkVertices.push_back({
						XMFLOAT3(xNeg,yNeg, zPos), blocks[x][y][z].colour,// 1
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos,zNeg), blocks[x][y][z].colour, // 2
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos, zPos), blocks[x][y][z].colour, // 3
					});

					/* +X */
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg,zNeg), blocks[x][y][z].colour, // 4
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg, zPos), blocks[x][y][z].colour, // 5
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos,zNeg), blocks[x][y][z].colour, // 6
					});

					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg, zPos), blocks[x][y][z].colour, // 5
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos, zPos), blocks[x][y][z].colour, // 7
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos,zNeg), blocks[x][y][z].colour, // 6
					});

					/* -Y */
					chunkVertices.push_back({
					XMFLOAT3(xNeg,yNeg,zNeg), blocks[x][y][z].colour, // 0
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg,yNeg, zPos), blocks[x][y][z].colour,// 1
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg, zPos), blocks[x][y][z].colour, // 5
					});

					chunkVertices.push_back({
					XMFLOAT3(xNeg,yNeg,zNeg), blocks[x][y][z].colour, // 0
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg, zPos), blocks[x][y][z].colour, // 5
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg,zNeg), blocks[x][y][z].colour, // 4
					});

					/* +Y */
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos,zNeg), blocks[x][y][z].colour, // 2
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos,zNeg), blocks[x][y][z].colour, // 6
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos, zPos), blocks[x][y][z].colour, // 7
					});

					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos,zNeg), blocks[x][y][z].colour, // 2
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos, zPos), blocks[x][y][z].colour, // 7
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos, zPos), blocks[x][y][z].colour, // 3
					});

					/* -Z */
					chunkVertices.push_back({
					XMFLOAT3(xNeg,yNeg,zNeg), blocks[x][y][z].colour, // 0
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg,zNeg), blocks[x][y][z].colour, // 4
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos,zNeg), blocks[x][y][z].colour, // 6
					});

					chunkVertices.push_back({
					XMFLOAT3(xNeg,yNeg,zNeg), blocks[x][y][z].colour, // 0
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos,zNeg), blocks[x][y][z].colour, // 6
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos,zNeg), blocks[x][y][z].colour, // 2
					});

					/* +Z */
					chunkVertices.push_back({
						XMFLOAT3(xNeg,yNeg, zPos), blocks[x][y][z].colour,// 1
					});
					chunkVertices.push_back({
						XMFLOAT3(xNeg, yPos, zPos), blocks[x][y][z].colour, // 3
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos, zPos), blocks[x][y][z].colour, // 7
					});

					chunkVertices.push_back({
						XMFLOAT3(xNeg,yNeg, zPos), blocks[x][y][z].colour,// 1
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos, yPos, zPos), blocks[x][y][z].colour, // 7
					});
					chunkVertices.push_back({
						XMFLOAT3(xPos,yNeg, zPos), blocks[x][y][z].colour, // 5
					});
				}
			}
		}
	}

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
				if (!blocks[x][y][z].isAir) {

					// Loop through adjacent blocks...
					bool shouldBeVisible = false;
					for (int ax = -1; ax <= 1; ax++) {
						for (int ay = -1; ay <= 1; ay++) {
							for (int az = -1; az <= 1; az++) {

								if (ax != x && ay != y && az != z) {
									if ((x + ax) >= 0 && (x + ax) <= 15 && (y + ay) >= 0 && (y + ay) <= 15 && (z + az) >= 0 && (z + az) <= 15)
									{
										if (blocks[x + ax][y + ay][z + az].isAir) {
											shouldBeVisible = true;
										}
									}
								}
							}
						}
					}

					if (x == 0 || x == CHUNK_SIZE.x-1 || y == 0 || y == CHUNK_SIZE.y-1 || z == 0 || z == CHUNK_SIZE.z-1)
					{
						shouldBeVisible = true;
					}

					blocks[x][y][z].isVisible = shouldBeVisible;
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