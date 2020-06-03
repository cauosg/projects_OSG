using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Candy_dispenser : MonoBehaviour {

    private List<List<int>> Tile_map;
    private int width, height;
    private List<int> dispense_pos;//from top
    // Use this for initialization
    void Start () {
        dispense_pos = new List<int>();

    }
	
    public void Receive_Tilemap(List<List<int>> input_map,int w, int h)
    {
        Tile_map = input_map;
        width = w;
        height = h;
    }

    public void Dispense_pos()
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {

                if (Tile_map[j][i] != 0)
                {
                    dispense_pos.Add(j);
                    Debug.Log("pos of dispense " + i + ": " + j);
                    break;
                }
            }
        }
    }

    public void Dispense()
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {

            }
        }
    }

    // Update is called once per frame
    void Update () {
		
	}
}
