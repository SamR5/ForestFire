# ForestFire

These are forest fire simulations. There is a grid of trees and at each step the fire propagates around.

There are two types of neighborhood:
  - the VON NEUMANN : North, East, South and West
  - the MOORE neighbors : N, NE, E, SE, S, SW, W and NW

________________
| NW | N  | NE |
|----+----+----|
| W  |FIRE| E  |
|----+----+----|
| SW | S  | SE |
________________


Each scripts has its specificity.

ForestFire(simulation):
  A rectangular grid filled with random trees (according to density) and a fire on the middle.
  The simulation is ran until the fire can't propagates anymore.
  The initial tree density, % of forest burnt and the total numbers of steps are written in a csv file.
  See the synthesis in the .xlsx file.

ForestFire:
  A rectangular grid with random trees that appears at each step (1 in p chance) and trees that ignite (1 in f chance)
  Usualy p=100 and f=1000.

ForestFire2:
  A rectangular grid with only the bottom line permanently on fire. The trees appears with 1 in p chance.

ForestFireHexa:
  An hexagonal gird (rectangular with odd rows offset by half-tile) that work with the same rules as ForestFire.
  There are two types of neighbors only because the indexes change on offset rows.

ForestFireTri:
  A triangular grid that behaves like the ForestFire with either 3 neighbors for the sides or 9 neighbors for the corners + 3 for the sides.
  
