import mymodule
from mymodule import *

def main():
    pet = Pet("Dog")
    print(pet.getName())
    pet.setName("Cat")
    print(pet.getName())
    print(mymodule.add(1, 2))
    
    
main()
