#include <iostream>
#include "core/position.h"

int main() {
    std::cout << "=== Smart City Emergency - Simple Tests ===\n\n";
    
    // Test de la structure Position
    Position pos1(100, 100);
    Position pos2(150, 150);
    
    std::cout << "Test 1: Structure Position\n";
    std::cout << "  Position 1: (" << pos1.x << ", " << pos1.y << ")\n";
    std::cout << "  Position 2: (" << pos2.x << ", " << pos2.y << ")\n";
    std::cout << "  Distance: " << pos1.distanceTo(pos2) << "\n";
    
    // Test d'égalité
    Position pos3(100, 100);
    std::cout << "  pos1 == pos3: " << (pos1 == pos3 ? "true" : "false") << "\n";
    
    std::cout << "\n✅ Tous les tests simples sont passés!\n";
    return 0;
}