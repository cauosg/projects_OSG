using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackTile_drawer : MonoBehaviour {
    /*
     Tile_map : 
     * 
        0 : none
        1 : hole
        2 : Blue
        3 : Red
        4 : Green
        5 : Yellow
        6 : Purple
        7 : Sausage
        8 : Candy ball
        9 : Rainbow
     Backtile : 
     * 0: A
     * 1: B
     * 2: hole
     */
    public GameObject[] Tiles;
    private float interval;
    private Vector2 center_point;
    private float cam_ratio;
    //private Vector2 level_res;
	// Use this for initialization
	void Start () {
		
	}
	
    private void Get_camsize(Vector2 res)
    {
        float cam_size = GameObject.Find("Main Camera").GetComponent<Camera>().orthographicSize;
        float horz_size = cam_size * res.x / res.y;
        interval = horz_size / 5;

    }

    private void Set_cp()
    {
        //interval = Mathf.Ceil(0.1f * Mathf.Min(res.x, res.y)) - 1;
        center_point.x = 0;
        center_point.y = - interval / 2 - interval*2;

        //Debug.Log(interval);
        //Debug.Log(center_point);
    }

    private Vector3 Get_tilepos(int i, int j,int BackTile_type)
    {
        float zorder = 0;
        if (BackTile_type > 1)
            zorder = - 0.1f;
        //cp-4step : origin
        //Vector3 out_pos;
        float x_pos = center_point.x - (4 - i) * interval;
        float y_pos = center_point.y - (j - 4) * interval;
        //Debug.Log(j.ToString()+ "," + i.ToString() + ":"+ x_pos.ToString() + "," + y_pos.ToString());
        return new Vector3(x_pos, y_pos, zorder);
    }

    public void Draw_BackTile(List<List<int>> Tile_map,int width, int height, Vector2 res)
    {
        //level_res = res;
        //Debug.Log(res);
        Get_camsize(res);
        Set_cp();
        bool check_marker = true;//false:B, true:A
        for(int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                check_marker = !check_marker;

                int tile_type = Tile_map[j][i];//transpose 안하는게 맞다
                if (tile_type < 1)
                    continue;

                int Backtile_type = 0;

                if (check_marker)
                    Backtile_type = 0;
                else
                    Backtile_type = 1;


                //Debug.Log(Get_tilepos(i, j, res));
                GameObject a_tile = Instantiate(Tiles[Backtile_type], Get_tilepos(i, j, Backtile_type), Quaternion.identity) as GameObject;
                a_tile.name = "[" + i + "][" + j + "]";             
                float scale_factor = interval/a_tile.GetComponent<SpriteRenderer>().bounds.size.x;
                a_tile.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
                a_tile.transform.SetParent(GameObject.Find("Back_Tile_parent").transform);

                if (tile_type == 1)
                {
                    Backtile_type = 2;
                    GameObject a_hole = Instantiate(Tiles[Backtile_type], Get_tilepos(i, j, Backtile_type), Quaternion.identity) as GameObject;
                    //a_hole.name = "[" + j + "][" + i + "]" + "hole";
                    a_hole.name = "hole";
                    float scale_hole = interval / a_hole.GetComponent<SpriteRenderer>().bounds.size.x;
                    a_hole.transform.localScale = new Vector3(scale_hole, scale_hole, scale_hole);
                    a_hole.transform.SetParent(a_tile.transform);
                }
                else//기본 캔디 배치 및 배열 넘겨주기
                {
                    GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Dispense_one(tile_type, Get_tilepos(i, 0, Backtile_type), Get_tilepos(i, j, Backtile_type), interval);
                }
            }
        }
        GameObject.Find("Level_manager").GetComponent<Level>().Set_interval(interval);

    }

	// Update is called once per frame
	void Update () {
		
	}
}
