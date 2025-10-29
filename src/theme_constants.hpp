#pragma once

#include <QStringList>

/*
 * theme_constants.hpp
 *
 * Defines a comprehensive list of background themes for asset categorization.
 * Users can search and select from these themes when adding background assets.
 */

namespace ThemeConstants
{
    // Comprehensive list of background themes
    inline QStringList getThemes()
    {
        return QStringList{
            // Natural Landscapes
            "Desert",
            "Forest",
            "Jungle",
            "Rainforest",
            "Tundra",
            "Mountain",
            "Hill",
            "Valley",
            "Canyon",
            "Cliff",
            "Cave",
            "Volcano",
            
            // Water & Wetlands
            "Ocean",
            "Sea",
            "Beach",
            "Coast",
            "Lake",
            "River",
            "Waterfall",
            "Swamp",
            "Marsh",
            "Wetland",
            
            // Grasslands & Fields
            "Meadow",
            "Prairie",
            "Savanna",
            "Grassland",
            "Field",
            "Farmland",
            
            // Cold Climates
            "Winter",
            "Snow",
            "Ice",
            "Glacier",
            "Arctic",
            "Antarctic",
            
            // Arid Climates
            "Arid",
            "Wasteland",
            "Badlands",
            "Dunes",
            
            // Urban & Built Environment
            "City",
            "Town",
            "Village",
            "Urban",
            "Suburban",
            "Industrial",
            "Futuristic",
            "Cyberpunk",
            "Steampunk",
            "Medieval",
            "Ancient",
            "Ruins",
            
            // Fantasy & Sci-Fi
            "Fantasy",
            "Magical",
            "Mystical",
            "Space",
            "Alien",
            "Post-Apocalyptic",
            "Dystopian",
            
            // Sky & Atmosphere
            "Sky",
            "Clouds",
            "Sunset",
            "Sunrise",
            "Night",
            "Starry",
            "Aurora",
            
            // Indoor
            "Interior",
            "Room",
            "Hall",
            "Dungeon",
            "Castle",
            "Palace",
            "Temple",
            "Laboratory",
            "Office",
            "Classroom",
            "Stage",
            
            // Seasonal & Weather
            "Spring",
            "Summer",
            "Autumn",
            "Fall",
            "Rainy",
            "Stormy",
            "Foggy",
            "Misty",
            
            // Other
            "Abstract",
            "Colorful",
            "Minimalist",
            "Pattern",
            "Texture",
            "Other"
        };
    }
}

