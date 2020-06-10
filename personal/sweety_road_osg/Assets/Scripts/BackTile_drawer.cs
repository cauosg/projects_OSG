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
    public float interval;
    public Vector2 center_point;
    public int queues = 7;
    private float cam_ratio;
	// Use this for initialization
	void Start () {
		
	}
	
    private void Get_camsize(Vector2 res)//종횡비
    {
        float cam_size = GameObject.Find("Main Camera").GetComponent<Camera>().orthographicSize;
        float horz_size = cam_size * res.x / res.y;
        interval = horz_size / 5;

    }

    private void Set_cp()//중앙설정, 정중앙보다 살짝 아래
    {
        //interval = Mathf.Ceil(0.1f * Mathf.Min(res.x, res.y)) - 1;
        center_point.x = 0;
        center_point.y = - interval / 2 - interval*2;

        //Debug.Log(interval);
        //Debug.Log(center_point);
    }

    private Vector3 Get_tilepos(int i, int j,int BackTile_type)//인덱스에 따른 transform 좌표 계산
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

    public void Draw_BackTile(List<List<int>> Tile_map,int width, int height, Vector2 res)//배경 타일만을 체크모양으로 그리며, csv에 저장된 정보에 따라 유연하게 사용 가능
    {
        Get_camsize(res);
        Set_cp();
        Candy_dispenser dispenser = GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>();
        dispenser.Receive_Tilemap(Tile_map, width, height,interval,center_point);
        //Debug.Log(res);
        //List<List<bool>> valid_space = new List<List<bool>>();
        bool check_marker = true;//false:B, true:A
        for(int i = 0; i < width; i++)
        {
            //List<bool> a_line_bool = new List<bool>();

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
                else//기본 캔디 배치 및 배열 넘겨주기, 배경타일은 캔디들과 연결점이 없으며, 개인적으로 행동, 질서를 위한 제어는 dispenser에서 수행
                {
                    dispenser.Dispense_one(tile_type, i, j, 0,queues);
                    
                    //a_line_bool.Add()
                }
            }
        }
        GameObject.Find("Level_manager").GetComponent<Level>().Set_interval(interval);

    }

    //public Vector2 Get_Centerpoint()
    //{

    //}

	// Update is called once per frame
	void Update () {
		
	}
}
