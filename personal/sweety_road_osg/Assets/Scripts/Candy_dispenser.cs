using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Candy_dispenser : MonoBehaviour {

    public GameObject _Candy;

    private List<List<int>> Tile_map;
    private int width, height;
    private List<int> dispense_pos;//from top
    private List<List<int>> Candy_pos;

    //forDebeug
    //private bool once = true;
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
                    //Debug.Log("pos of dispense " + i + ": " + j);
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

                int candy_type = Tile_map[i][j];//transpose
                if (candy_type < 2)
                    continue;


                //Debug.Log(Get_tilepos(i, j, res));
                //GameObject a_candy = Instantiate(_Candy, new Vector3(0,0,0), Quaternion.identity) as GameObject;
               // a_candy.name = "[" + j + "][" + i + "]";
                //float scale_factor = interval / a_candy.GetComponent<SpriteRenderer>().bounds.size.x;
                //a_candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
                //a_candy.transform.SetParent(GameObject.Find("Back_Tile_parent").transform);
            }
        }
    }

    public void Dispense_one(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interval,int i, int j)
    {
        //fordebug
        //note, contructor사용 불가하므로 init 만들것
        //if (once)
        //{
        //Vector2 begin_pos = new Vector2(0, 0);
        GameObject a_candy = Instantiate(_Candy);
        a_candy.GetComponent<Candy>().Init(candy_type, begin_pos, target_pos, interval,i,j, "Candy[" + i + "][" + j + "]");
        a_candy.name = "Candy[" + i + "][" + j + "]";
        //Candy a_candy = Instantiate(_Candy).GetComponent<Candy>();
        //a_candy(candy_type, begin_pos, target_pos);
        //once = false;
        //}
    }

    // Update is called once per frame
    void Update () {
		
	}
}
