using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Candy_dispenser : MonoBehaviour {

    public GameObject _Candy;

    public bool is_drop = true;
    public bool is_process = false;
    public bool is_update = false;
    public bool is_move = false;
    public bool fired = false;
    public List<Candy> powders;
    public List<List <Candy>> explode_queue;
    private List<int> scores_queue;

    private List<List<int>> Tile_map,empty_pos;
    private List<string> dispense_candies_names;
    private int width, height;
    private int[] pos_ind, valid_candies;

    //private List<List<GameObject>> tot_candies;

    private List<int> dispense_pos, dispense_candies;//from top
    //private List<GameObject> dispense_candies;
    //private List<int>[] dispense_candies;//,Candy_pos;
    //private List<int> ;
    

    private Vector2 center_point;
    private float interval;
    //private bool is_drop = true;

    //forDebeug
    //private bool once = true;
    // Use this for initialization
    void Start () {
        dispense_pos = new List<int>();
        dispense_candies = new List<int>();
        dispense_candies_names = new List<string>();
        empty_pos = new List<List<int>>();

        pos_ind = new int[2];
        //dispense_candies = new List<>();
    }

    public void Init()
    {
        
    }

    public void Receive_Tilemap(List<List<int>> input_map,int w, int h, float interv, Vector2 cp)
    {
        this.center_point = GameObject.Find("BackTile_drawer").GetComponent<BackTile_drawer>().center_point;
        this.interval = GameObject.Find("BackTile_drawer").GetComponent<BackTile_drawer>().interval;
        //Debug.Log("dispensers interval is " + interval);
        //Debug.Log("dispensers cp is " + center_point);

        Tile_map = input_map;
        width = w;
        height = h;
        valid_candies = GameObject.Find("Level_manager").GetComponent<Level>().valid_candies;
        //dispense_candies = new List<int>[width];
        Debug.Log(dispense_candies);
        //Debug.Log(dispense_candies.Length);
        Reset_fills();
    }

    private void Reset_fills()
    {
        
        empty_pos.Clear();
        dispense_candies_names.Clear();
        //Debug.Log("ep count of " + 0 + " is " + empty_pos.Count);
        // for(int i = 0; i< empty_pos[i].Count;i++)
        //  Debug.Log("ep count of " + i + " is " + empty_pos[i].Count);
        dispense_candies.Clear();
        for (int i = 0; i < width; i++)
        {
            dispense_candies.Add(0);
            empty_pos.Add(new List<int>());
        }
       
    }

    public void Dispense_pos()
    {
        for (int i = 0; i < width; i++)
        {
            //empty_pos.Add(new List<int>());
            dispense_candies.Add(0);
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
        Debug.Log(dispense_candies.Count);
    }

    //public void Dispense()
    //{
    //    for (int i = 0; i < width; i++)
    //    {
    //        List<int> Temp = new List<int>();
    //        Temp.Add(0);
    //        empty_pos.Add(Temp);
    //        for (int j = 0; j < height; j++)
    //        {

    //            int candy_type = Tile_map[i][j];//transpose
    //            if (candy_type < 2)
    //                continue;
    //        }
    //    }
    //}

    public void Dispense_one(int candy_type, int i, int j, int origin, int ques)
    {
        //fordebug
        //note, contructor사용 불가하므로 init 만들것
        //Debug.Log("this candy is [" + i + "][" + j + "]");

        GameObject a_candy = Instantiate(_Candy);
        a_candy.GetComponent<Candy>().Init(candy_type, Get_candypos(i,origin), Get_candypos(i,j), interval,i, j, ques);
    }

    private Vector3 Get_candypos(int i, int j)
    {
        float zorder = -0.2f;
        //cp-4step : origin

        float x_pos = center_point.x - (4 - i) * interval;
        float y_pos = center_point.y - (j - 4) * interval;

        return new Vector3(x_pos, y_pos, zorder);
    }
    public void Refill_plz(Candy in_candy)
    {
        if (dispense_candies_names.Contains(in_candy.name))
            return;

        dispense_candies_names.Add(in_candy.name);

        dispense_candies[in_candy.Get_ind_x()]++;
        //Debug.Log(empty_pos.Count);
        empty_pos[in_candy.Get_ind_x()].Add(in_candy.Get_ind_y());
        //Debug.Log("Candy Refill is requested at [" + i + "][" + j + "]");
    }



    public void Explode_candies()
    {
        List<string> broken_candies = new List<string>();
        for (int i = 0; i< powders.Count; i++)
        {
            //Debug.Log("Candy will destroyed is " + powders[i].gameObject.name);
            if (broken_candies.Contains(powders[i].name))
                continue;
            broken_candies.Add(powders[i].gameObject.name);
            powders[i].Bomb();            
        }
        powders.Clear();
        //Reset_fills();
    }

    public void Candy_drop()
    {
        if (is_drop)
            return;
        //for (int i = 0; i < empty_pos.Count; i++)
        //    Debug.Log("ep count of " + i + " is " + empty_pos[i].Count);
        for (int i = 0; i<width; i++)//width : 9, candy는 1~8
        {
            //Debug.Log("dispense candies : " + dispense_candies[i]);
            if (dispense_candies[i] < 1)
                continue;
            
            int[] move_steps = new int[height];

            int max_pos = 0;
            //Debug.Log("ep count of " + i + " is " + empty_pos[i].Count);
            for (int j = 0; j < empty_pos[i].Count; j++)
            {
                for (int k = 0; k < empty_pos[i][j]; k++)
                {
                    move_steps[k]++;
                    if (k > max_pos)
                        max_pos = k;
                }
            }
            int max_steps = 0;
            for (int j = 0; j < height; j++)//max까지 포함하기 위해 +1
            {
                int reverse_j = height - j;
                string drop_candy_name = "Candy[" + i + "][" + reverse_j + "]";
                
                GameObject temp = GameObject.Find(drop_candy_name);
                if (temp == null)
                    continue;
                //Debug.Log("candy will be moved is " + drop_candy_name);
                Candy now_candy = temp.GetComponent<Candy>();
                now_candy.Move_to_new(i, reverse_j + move_steps[reverse_j], Get_candypos(i, reverse_j + move_steps[reverse_j]), move_steps[reverse_j]);
                //Debug.Log("------move_succeed " + drop_candy_name + "to [" + i + "][" + (reverse_j + move_steps[reverse_j]) + "]");
                if (max_steps < move_steps[reverse_j])
                    max_steps = move_steps[reverse_j];
            }
            //Debug.Log("max step is " + max_steps);
            for (int j = 0; j < dispense_candies[i]; j++)
            {
                Dispense_one(valid_candies[Random.Range(0,valid_candies.Length-1)],i,j + 1, 0 , move_steps[i]+1);
            }
        }


        Reset_fills();
        is_update = true;
        is_drop = true ;//1회실행

    }

    // Update is called once per frame
    void Update () {
        if (!is_drop)
            Candy_drop();
        //if (fired) {
        //    Explode_candies();
        //    powders.Clear();
        //    fired = false;
        //}
	}
}
