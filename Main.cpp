#include <SFML/Graphics.hpp>
#include <vector>


class GravitySource
{
    sf::Vector2f pos;
    float strength;
    sf::CircleShape s;

public:
    GravitySource(float pos_x, float pos_y, float strength)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(20);
        s.setOrigin(10, 10); 
    }

    void render(sf::RenderWindow& wind)
    {
        wind.draw(s);
    }

    sf::Vector2f get_pos()
    {
        return pos;
    }

    float get_strength()
    {
        return strength;
    }

    bool contains(sf::Vector2f point)
    {
        // Check if the point is within the bounds of the gravity source
        float dx = point.x - pos.x;
        float dy = point.y - pos.y;
        return (dx * dx + dy * dy) <= (s.getRadius() * s.getRadius());
    }
};


class Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;

public:
    Particle(float pos_x, float pos_y, float vel_x, float vel_y)
    {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);

        s.setRadius(6);
    }

    void render(sf::RenderWindow& wind)
    {
        s.setPosition(pos);
        wind.draw(s);
    }

    void set_color(sf::Color col)
    {
        s.setFillColor(col);
    }

    void update_physics(GravitySource& s)
    {
        float distance_x = s.get_pos().x - pos.x;
        float distance_y = s.get_pos().y - pos.y;

        float distance = sqrt(distance_x * distance_x + distance_y * distance_y);

        float inverse_distance = 1.f / distance;

        float normalized_x = inverse_distance * distance_x;
        float normalized_y = inverse_distance * distance_y;

        float inverse_square_dropoff = inverse_distance * inverse_distance;

        float acceleration_x = normalized_x * s.get_strength() * inverse_square_dropoff;
        float acceleration_y = normalized_y * s.get_strength() * inverse_square_dropoff;

        vel.x += acceleration_x;
        vel.y += acceleration_y;

        pos.x += vel.x;
        pos.y += vel.y;
    }
};


sf::Color map_val_to_color(float value) //value is 0-1
{
    if (value < 0.0f) value = 0;
    if (value > 1.0f) value = 1;

    int r = 0, g = 0, b = 0;

    if (value < 0.5f)
    {
        b = 255 * (1.0f - 2 * value);
        g = 255 * 2 * value;
    }
    else {
        g = 255 * (2.0f - 2 * value);
        r = 255 * (2 * value - 1);
    }
    return sf::Color(r, g, b);

}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "thvbfghb");

    // Enable V-Sync
    //window.setVerticalSyncEnabled(true);

    window.setFramerateLimit(60);

    std::vector<GravitySource> sources;

    int num_particles = 2000;

    std::vector<Particle> particles;

    bool isPaused = false;
    bool keyPressed = false;
    

    

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        }

        if (!(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            sources.push_back(GravitySource(mousePosF.x, mousePosF.y, 7000));
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            auto it = sources.begin();
            while (it != sources.end())
            {
                if (it->contains(mousePosF))
                {
                    it = sources.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            sources.clear();
            particles.clear();
        }
  

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            particles.push_back(Particle(mousePosF.x, mousePosF.y, 4, 0.2 + (0.1 / num_particles)));
        }

        window.clear();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            if (!keyPressed) {
                isPaused = !isPaused;
                keyPressed = true;
            }
        }
        else {
            keyPressed = false;
        }


        if (!isPaused) {
            for (int i = 0; i < sources.size(); i++) {
                for (int j = 0; j < particles.size(); j++)
                {
                    particles[j].update_physics(sources[i]);
                }
            }
        }

        for (int i = 0; i < sources.size(); i++) {
            sources[i].render(window);
        }

        for (int j = 0; j < particles.size(); j++)
        {
            particles[j].render(window);
            float val = (float)j / (float)num_particles;
            sf::Color col = map_val_to_color(val);
            particles[j].set_color(col);
        }

        window.display();
    }

    return 0;
}