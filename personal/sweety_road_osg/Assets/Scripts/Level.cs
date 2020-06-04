using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Level : MonoBehaviour {

    public float Interval;// { get { return Interval; } set { Interval = value; } }//stack overflow발생

    private List<List<int>> Tile_map;
    private int BackTile_width, BackTile_height;
    private Vector2 level_res;//;// { get { return level_res; } set { } }
	// Use this for initialization
	void Start () {

        GameObject.Find("Tile_loader").GetComponent<Tile_loader>().Init();
        Tile_map = GameObject.Find("Tile_loader").GetComponent<Tile_loader>().Get_Tile_map();
        BackTile_width = GameObject.Find("Tile_loader").GetComponent<Tile_loader>().Get_width();
        BackTile_height = GameObject.Find("Tile_loader").GetComponent<Tile_loader>().Get_height();
        Set_res();
        GameObject.Find("BackTile_drawer").GetComponent<BackTile_drawer>().Draw_BackTile(Tile_map,BackTile_width,BackTile_height,level_res);
        GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Receive_Tilemap(Tile_map, BackTile_width, BackTile_height);
        GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Dispense_pos();
    }

    //-----stack overflow 이유 추후 찾아보기
    public void Set_interval(float interv)
    {
        this.Interval = interv;
    }
    public float Get_interval()
    {
        return this.Interval;
    }
    //-------------
    private void Set_res()
    {
        level_res.x = 720;
        level_res.y = 1280;
    }
    public Vector2 Get_res()
    {
        return level_res;
    }
	// Update is called once per frame
	void Update () {
		
	}
}
