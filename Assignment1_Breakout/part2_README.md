# Breakout - Part 2

<img align="right" width="150px" src="./Media/Nolan_Bushnell_2013.jpg">
Nolan is impressed! You have built your first prototype of the game in part 1 and it is starting to come together beautifully.

Local game review sites [Gamespot](https://www.gamespot.com/) and [IGN](https://www.ign.com/) have rated your game preview a 9 out of 10! The hype for your game is motivating the team to work extra hours! Now it is time to clean up the code to produce more final product.

## Step 1 - Resource Manager

Nolan thinks the game may be running on more resource constrained machines than originally intended. He would like a [Singleton](https://en.wikipedia.org/wiki/Singleton_pattern) class for a resource manager.  *You then do a quick doodle on the white board of pseudo-code for a Resource Manager*

```c
class ResourceManager{
  private:
    ResourceManager();                      // Private Constructor
    ResourceManager(ResourceManger const&); // Avoid copy constructor
    void operator=(ResourceManager const&); // Don't allow assignment.
    // ... // perhaps other private members
    
  public:
  
  static ResourceManager &getInstance();
  
  // 'equivalent' to our constructor
  int startUp();
  
  // 'equivalent' to our destructor
  int shutDown();
  
  // ... // Perhaps other public functions..

}:
```

Nolan nods slowly and agrees that you may further look to previous lab and lecture materials for further inspiration.

## Step 2 - Frame Capping
The breakout game should be framecapped to 60fps as the number of machines we are targeting has grown. We want to make sure our game runs consistently across all platforms.

You decide you may also want to cleanup your game, as most of the logic, updates, and game code all exists in one giant loop. You start whiteboarding ideas about a timer class that may be helpful for frame capping and frame independent movement... :)

## Step 3 - More Levels
The main constructive critism from [Gamespot](https://www.gamespot.com/) and [IGN](https://www.ign.com/) was that the game was too short--it was fun, but just way to short. In order to combat this, there should now be at least three levels with potentially more if the reviewers are not satisfied..

Nolan is requiring that the levels are created from some sort of configuration file in case more are asked for later. That means, your game must not have any of the levels hardcoded. For now, Nolan would like 3 different levels, where each level has a different pattern with the bricks.

You again go to your whiteboard to think about a format for loading level data into your game. Perhaps a simple text-based file would work, you start to sketch something out below.

```
00000000000000000
11111111111111111
11111111111111111
11111111111111111
00000000000000000
00000000000000000
00000000000000000
00000000000000000
00000000000000000
00000000000000000
```

## Step 4 - Localization
The light bulb also goes off as Nolan is discussing the idea of configuration files for not just the levels, but for other languages. He decides breakout should be played by everyone--in the world! You calm Nolan down a little bit, and settle on supporting one other langauge with the help of a tool like [Google Translate](https://translate.google.com/).

Nolan wants all strings in your game (e.g. "score", "level", etc.) to be loaded from a separate file. You will now support at least two languages of your choice (e.g. English and French, English and Japanese, English and Hindi, English and Klingon, etc.) that can be selected before you start the game.

## More Resources

* [Game Loop Pattern](http://www.gameprogrammingpatterns.com/game-loop.html)
* [Flyweight Pattern](http://www.gameprogrammingpatterns.com/flyweight.html)
* On localization
  * [What Every Programmer Absolutely, Positively Needs To Know About Encodings And Character Sets To Work With Text](http://kunststube.net/encoding/)
  * [utf-8 everywhere](http://utf8everywhere.org/)
  
