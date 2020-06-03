using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackTile_drawer_test : MonoBehaviour {
    /*
     Tile_map : 
     * 0: nothing
     * 1: normal(A,B)
     * 2: hole
     Backtile : 
     * 0: A
     * 1: B
     * 2: hole
     */
    public GameObject[] Tiles;
    private float interval;
    private Vector2 center_point;
    private float cam_ratio = 3.00000f / 720.00000f;
    //private Vector2 level_res;
	// Use this for initialization
	void Start () {
		
	}
	
    private void Get_camsize()
    {
        float todebug = GameObject.Find("Main Camera").GetComponent<Camera>().orthographicSize;
        Debug.Log(todebug);
    }

    private void Set_cp(Vector2 res)
    {
        interval = Mathf.Ceil(0.1f * Mathf.Min(res.x, res.y)) - 1;
        center_point.x = res.x - interval / 2;
        center_point.y = res.y - interval / 2 - interval*2;
        Debug.Log(cam_ratio);
    }

    private Vector3 Get_tilepos(int i, int j, Vector2 res)
    {
        float zorder = 0;
        //cp-4step : origin
        //Vector3 out_pos;
        float x_pos = center_point.x - (i - 4) * interval;
        float y_pos = center_point.y - (j - 4) * interval;

        return new Vector3(cam_ratio * x_pos, cam_ratio * x_pos, zorder);
    }

    public void Draw_BackTile(List<List<int>> Tile_map,int width, int height, Vector2 res)
    {
        //level_res = res;
        //Debug.Log(res);
        Get_camsize();
        Set_cp(res);
        bool check_marker = true;//false:B, true:A
        for(int j = 0; j< height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                int tile_type = Tile_map[j][i];
                if (tile_type < 1)
                    continue;

                int Backtile_type = 0;
                check_marker = !check_marker;

                if (tile_type > 1)
                    Backtile_type = 2;
                else
                {
                    if (check_marker)
                        Backtile_type = 0;
                    else
                        Backtile_type = 1;
                }
                //Debug.Log(Get_tilepos(i, j, res));
                GameObject a_tile = Instantiate(Tiles[Backtile_type], transform.position + Get_tilepos(i, j, res), Quaternion.identity);
                a_tile.name = "[" + i + "][" + j + "]";
                Vector3 todebug = a_tile.GetComponent<SpriteRenderer>().bounds.size;
                Debug.Log(todebug);
                //a_tile.transform.localScale = new Vector3(cam_ratio);
            }
        }
    }

	// Update is called once per frame
	void Update () {
		
	}
}
