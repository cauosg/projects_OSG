﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class Tile_loader_test : MonoBehaviour {

    public string Tile_level = "";

    private string base_path;

    private List<string> Tile_map = new List<string>();

	// Use this for initialization
	void Start () {

        
        base_path = Application.dataPath + "/Tiles/";
        Debug.Log(base_path + Tile_level + ".csv");

        StreamReader reader = new StreamReader(base_path + Tile_level + ".csv");

        do
        {
            string a_line = reader.ReadLine();
            if (a_line == null)
                break;
            Debug.Log(a_line.Length);
            //for(int i = 0;i<a_line.Length)

            Tile_map.Add(a_line);
        } while (true);

        //Debug.Log(Tile_map.Count);
        //int ind = 0;
        //while (ind < Tile_map.Count)
        //{
        //    Debug.Log(Tile_map[ind++]);
        //}
        //string str_array = Tile_map.ToArray().ToString();
        
        //Debug.Log(str_array);
    }
	
    //int[][] Tile_Array(List<string> Tile_map)
    //{
    //    string str_array = Tile_map.ToArray().ToString();
    //    int [][]
    //}

	// Update is called once per frame
	void Update () {
		
	}
}
