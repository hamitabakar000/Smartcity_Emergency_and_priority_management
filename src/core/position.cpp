// Fichier position.cpp - version minimale
// Créé pour résoudre l'erreur CMake

#include <cmath>

class Position {
private:
    float x, y;
public:
    Position() : x(0), y(0) {}
    Position(float x, float y) : x(x), y(y) {}
    
    float getX() const { return x; }
    float getY() const { return y; }
    
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
    
    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }
};

// Point d'entrée minimal si nécessaire
void dummy_position_function() {
    Position p;
    p.move(1, 1);
}
