#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct PuntoTuristico {
    std::string nombre;
    sf::Vector2f coordenadas;
    PuntoTuristico* anterior;
    PuntoTuristico* siguiente;
    PuntoTuristico(std::string n, sf::Vector2f c) : nombre(n), coordenadas(c), anterior(nullptr), siguiente(nullptr) {}
};

class RutaTuristica {
public:
    std::string nombre;
    PuntoTuristico* inicio;
    PuntoTuristico* fin;
    sf::Color color;

    RutaTuristica(std::string n, sf::Color c) : nombre(n), inicio(nullptr), fin(nullptr), color(c) {}

    void agregarPunto(const std::string& nombre, const sf::Vector2f& coordenadas) {
        PuntoTuristico* nuevo = new PuntoTuristico(nombre, coordenadas);
        if (!inicio) {
            inicio = fin = nuevo;
        }
        else {
            fin->siguiente = nuevo;
            nuevo->anterior = fin;
            fin = nuevo;
        }
    }

    void dibujarRuta(sf::RenderWindow& window) {
        if (!inicio) return;
        std::vector<sf::Vector2f> puntos;
        for (PuntoTuristico* actual = inicio; actual != nullptr; actual = actual->siguiente) {
            puntos.push_back(actual->coordenadas);
        }
        sf::VertexArray curva(sf::LineStrip, puntos.size());
        for (size_t i = 0; i < puntos.size(); ++i) {
            curva[i].position = puntos[i];
            curva[i].color = color;
        }
        window.draw(curva);
        for (PuntoTuristico* actual = inicio; actual != nullptr; actual = actual->siguiente) {
            sf::CircleShape punto(5);
            punto.setFillColor(color);
            punto.setPosition(actual->coordenadas.x - 5, actual->coordenadas.y - 5);
            window.draw(punto);
            sf::Font font;
            if (font.loadFromFile("arial.ttf")) {
                sf::Text texto(actual->nombre, font, 12);
                texto.setPosition(actual->coordenadas);
                texto.setFillColor(sf::Color::White);
                window.draw(texto);
            }
        }
    }

    ~RutaTuristica() {
        while (inicio) {
            PuntoTuristico* temp = inicio;
            inicio = inicio->siguiente;
            delete temp;
        }
    }
};

void guardarRutas(const std::vector<RutaTuristica*>& rutas) {
    for (size_t i = 0; i < rutas.size(); ++i) {
        std::ofstream archivo("ruta_" + std::to_string(i) + ".txt");
        archivo << rutas[i]->nombre << "\n";
        for (PuntoTuristico* actual = rutas[i]->inicio; actual != nullptr; actual = actual->siguiente) {
            archivo << actual->nombre << " " << actual->coordenadas.x << " " << actual->coordenadas.y << "\n";
        }
    }
}

void cargarRutas(std::vector<RutaTuristica*>& rutas) {
    for (int i = 0; ; ++i) {
        std::ifstream archivo("ruta_" + std::to_string(i) + ".txt");
        if (!archivo.is_open()) break;
        std::string nombreRuta;
        std::getline(archivo, nombreRuta);
        RutaTuristica* ruta = new RutaTuristica(nombreRuta, sf::Color::Red);
        std::string nombrePunto;
        float x, y;
        while (archivo >> nombrePunto >> x >> y) {
            ruta->agregarPunto(nombrePunto, sf::Vector2f(x, y));
        }
        rutas.push_back(ruta);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Rutas Turisticas");
    sf::Texture mapaTexture;
    if (!mapaTexture.loadFromFile("mapa.png")) {
        return -1;
    }
    sf::Sprite mapaSprite(mapaTexture);
    std::vector<RutaTuristica*> rutas;
    cargarRutas(rutas);
    RutaTuristica* rutaActual = nullptr;
    sf::Color colorActual = sf::Color::Green;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                guardarRutas(rutas);
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (rutaActual) {
                    float x = static_cast<float>(event.mouseButton.x);
                    float y = static_cast<float>(event.mouseButton.y);
                    std::string nombrePunto;
                    std::cout << "Nombre del punto: ";
                    std::cin >> nombrePunto;
                    rutaActual->agregarPunto(nombrePunto, sf::Vector2f(x, y));
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::N) {
                    std::string nombreRuta;
                    std::cout << "Nombre de la nueva ruta: ";
                    std::cin >> nombreRuta;
                    rutaActual = new RutaTuristica(nombreRuta, colorActual);
                    rutas.push_back(rutaActual);
                }
                if (event.key.code == sf::Keyboard::C) {
                    colorActual = sf::Color::Blue;
                }
            }
        }
        window.clear();
        window.draw(mapaSprite);
        for (auto& ruta : rutas) {
            ruta->dibujarRuta(window);
        }
        window.display();
    }

    for (auto& ruta : rutas) {
        delete ruta;
    }
    return 0;
}

