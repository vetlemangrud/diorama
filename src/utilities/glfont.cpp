#include <iostream>
#include "glfont.h"

Mesh generateTextGeometryBuffer(std::string text, float characterHeightOverWidth, float totalTextWidth) {
    float characterWidth = totalTextWidth / float(text.length());
    float characterHeight = characterHeightOverWidth * characterWidth;

    unsigned int vertexCount = 4 * text.length();
    unsigned int indexCount = 6 * text.length();

    Mesh mesh;

    mesh.vertices.resize(vertexCount);
    mesh.indices.resize(indexCount);
    mesh.textureCoordinates.resize(vertexCount);

    for(unsigned int i = 0; i < text.length(); i++)
    {
        float baseXCoordinate = float(i) * characterWidth;

        mesh.vertices.at(4 * i + 0) = {baseXCoordinate, 0, 0};
        mesh.vertices.at(4 * i + 1) = {baseXCoordinate + characterWidth, 0, 0};
        mesh.vertices.at(4 * i + 2) = {baseXCoordinate + characterWidth, characterHeight, 0};

        mesh.vertices.at(4 * i + 0) = {baseXCoordinate, 0, 0};
        mesh.vertices.at(4 * i + 2) = {baseXCoordinate + characterWidth, characterHeight, 0};
        mesh.vertices.at(4 * i + 3) = {baseXCoordinate, characterHeight, 0};


        mesh.indices.at(6 * i + 0) = 4 * i + 0;
        mesh.indices.at(6 * i + 1) = 4 * i + 1;
        mesh.indices.at(6 * i + 2) = 4 * i + 2;
        mesh.indices.at(6 * i + 3) = 4 * i + 0;
        mesh.indices.at(6 * i + 4) = 4 * i + 2;
        mesh.indices.at(6 * i + 5) = 4 * i + 3;

        // Setup texture coordinates
        // Get ASCII value
        unsigned int ascii = text.at(i);

        //Assume the texture contains all ascii chars in order, such that U coord 0 is ascii value 0 and U coord 127/128 is ascii value 127.
        float charWidthInTexture = (1.0/128.0);
        float charHeightInTexture = 1.0;
        float baseUCoordinate = ascii * charWidthInTexture;

        // Repeat process for setting vertex coordinates, just using UV.
        mesh.textureCoordinates.at(4 * i + 0) = {baseUCoordinate, 0};
        mesh.textureCoordinates.at(4 * i + 1) = {baseUCoordinate + charWidthInTexture, 0};
        mesh.textureCoordinates.at(4 * i + 2) = {baseUCoordinate + charWidthInTexture, charHeightInTexture};

        mesh.textureCoordinates.at(4 * i + 0) = {baseUCoordinate, 0};
        mesh.textureCoordinates.at(4 * i + 2) = {baseUCoordinate + charWidthInTexture, charHeightInTexture};
        mesh.textureCoordinates.at(4 * i + 3) = {baseUCoordinate, charHeightInTexture};

    }

    return mesh;
}
