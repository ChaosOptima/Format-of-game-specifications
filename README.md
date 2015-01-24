##Grammar
```FOGS file = {object}```

###Object
```
node = 
  name + optional(attributes) + optional( = value) ||
  attributes + optional( = value) ||
  value

object = 
  node + ; ||
  node + { repeate(object) }
``` 
###Name
```
Name can contain any characters exept
  [ - It creates meta value.
  " - If in begining, it creates string value
  = , {} () ; - Control chars.
```
###Attributes
```
attribute = 
  name + optional( = value)||
  value
  
attributes = ( + repeate(attribute + optional_at_end(;)) + )
```
###Value
```
Int Value = [1-0] 
Float Value = [1-0.1-0] 
String Value = "string" 
Bool Value = true, false, on, off, yes, no 
Constant Value = Name
Meta Value = Name[string]
Array = repeate(Value + exept_end(,))
```
##Example
```C++
Ignore everything before
{
	/*
	Support multiline comments
	*/

	Node = 10; // Node with name Node and value is 10
	Node = 10, 12, "string"; // Node with name Node and value as array
	Node; // Node with no name and constant value Node
	10; // Node with no name and value is 10

	Node // Node with name Node and sub nodes
	{
		Sub node;
		Sub node2
		{
			Sub sub node;
		}
	}


	Node(attr = 13) = 10;// Node with name Node and value 10 and with attribute which name is attr and value is 13
	Node(attr = 13; attr = 666) = 10;// Node with name Node and value 10 and with attributes
	(attr = 13) = 10;// Node with no name and value 10 and with attribute
	(13) = 10;// Node with no name and value 10 and with attribute which value is 13


	location (Type = Open; Difficulty = Easy) = "Start location."
	{
		Raw = Base64
[TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz
IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg
dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu
dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo
ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=];
	
		scripts
		{
			"./assets/start.lua";
			"./assets/player.lua";
			"./assets/enemy.lua";
		}
	
		object = "Tree"
        {
            position = 56.42, 23.0, 234.325;
            rotation = 0, 45.34, 0;
            
            mesh (layout = pos, UV, norm, tang, wei, ind; animated = true) 
                = "./assets/meshes/tree.ms";
                
            animations = "idle_1", "idle_2", "idle_3";
            
            collider (static = true) = cylinder
            {
                offset = 0, 5, 0;
                radius = 45.351;
                height = 180.45;
            }
            
            material = cook torrance
            {
                albedo = "./assets/textures/tree_a.png";
                normal(normal = rgb; spec = a) = "./textures/tree_n.png";
            }
            
            object = "leaves"
            {
                mesh (tangents = on; animated = true) = "./assets/meshes/leaves.ms";
                animations = "idle_1", "idle_2", "idle_3";
                
                material(translucent = true; alpha = alpha test) = cook torrance
                {
                    albedo = "./assets/textures/leaves_a.png";
                    normal(normal = rgb; spec = a) = "./textures/leaves_n.png";
                    translucent = "./textures/leaves_t.png";
                }
            }
        }	
    }

}
Ignore everything after
```
