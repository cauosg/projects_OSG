using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class Tile_loader : MonoBehaviour {

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
            Tile_map.Add(a_line);
        } while (true);

        //TextAsset _txtFile = (TextAsset)Resources.Load(base_path + Tile_level + ".csv") as TextAsset;
        //TextAsset _txtFile = (TextAsset)Resources.Load("level6.csv") as TextAsset;
        //string Tile_info = _txtFile.text;

        //Debug.Log(Tile_info);
        //StringReader lines = new StringReader(base_path + Tile_level + ".csv");

        //Debug.Log(lines.ReadLine());

        //int ind = 0;
        //do
        //{
        //    Tile_map[ind] = lines.ReadLine();
        //}
        //while (Tile_map[ind] != null);
        int ind = 0;
        while (ind < Tile_map.Count)
        {
            Debug.Log(Tile_map[ind++]);
        }
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
