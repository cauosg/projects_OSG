using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Level : MonoBehaviour {

    public float Interval;// { get { return Interval; } set { Interval = value; } }//stack overflow발생
    public int BackTile_width, BackTile_height;
    private List<List<int>> Tile_map;
    public int[] valid_candies;
    private Vector2 level_res;//;// { get { return level_res; } set { } }
        /*
    Tile_map : 
    * 
        0 : none
        1 : hole
        2 : Blue `
        3 : Red `
        4 : Green `
        5 : Yellow `
        6 : Purple `
        7 : Sausage
        8 : Candy ball
        9 : Rainbow
    Backtile : 
    * 0: A
    * 1: B
    * 2: hole
    */
        // Use this for initialization

    void Start () {
        Application.targetFrameRate = 60;
        Tile_loader loader = GameObject.Find("Tile_loader").GetComponent<Tile_loader>();
        loader.Init();
        Tile_map = loader.Get_Tile_map();
        BackTile_width = loader.Get_width();
        BackTile_height = loader.Get_height();
        Destroy(loader.gameObject);

        Set_res();
        BackTile_drawer drawer = GameObject.Find("BackTile_drawer").GetComponent<BackTile_drawer>();
        drawer.Draw_BackTile(Tile_map,BackTile_width,BackTile_height,level_res);
        Destroy(drawer.gameObject);
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
        level_res.x = Screen.width;
        level_res.y = Screen.height;
    }
    public Vector2 Get_res()
    {
        return level_res;
    }

	// Update is called once per frame
	void Update () {
		
	}
}
