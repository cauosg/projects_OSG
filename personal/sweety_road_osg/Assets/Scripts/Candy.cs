﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy : MonoBehaviour {

    public GameObject[] Candies;

    public int queues = 7;
    public float accel_factor = 0.00f;//1000으로 나눠줄 예정
    public float max_spd = 0.2f;
    public float zorder = -0.2f;
    public float Stop_anim_length = 1.0f;
    public float Stop_anim_spd = 1.0f;
    public float Stop_anim_power = 0.2f;//10으로 나눔
    public float stay_range = 0.2f;
    public float change_spd = 1.0f;
    public float change_sensitivity = 4;

    private GameObject its_Candy;
    private int this_type;
    private bool is_lerp, update_neighbors,is_show,is_stop_anim,is_moved,allow_change,is_change = false;
    private float spd = 0.000f;
    private float Stop_anim_time = 0.00f;
    private float scale_factor = 0;
    private float interval;
    
    public enum dir_4 {N, S, W, E };
    private string[] neighbors;
    private string its_name,name_to_change;
    private dir_4 now_dir;
    private bool[] movablity;
    private int ind_y, ind_x, width, height, next_y, next_x;
    private Vector2 begin_touch,end_touch, now_touch;
    private Vector3 dest_pos, now_pos, other_pos, change_dir;
    private int row_score, col_score;
    /*
     * type :
     basic:
     * B(blue) - 0
     * R(red) - 1
     * G(green) - 2
     * Y(yellow) - 3
     * P(Purple) - 4
     * S(sausage) - 5
     special:
     * C(ball candy)
     * Rainbow
     additional:
     * ribbon x,y
         
     */
    private void OnMouseDown()
    {
        Debug.Log(this.gameObject.name);
        //Debug.Log("x:" + ind_x + ", y:" + ind_y);
        Check_score();
        begin_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
    }
    private void OnMouseUp()
    {
        end_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        //if (!is_moved)
        //    this.transform.position = now_pos;// new Vector3(now_pos.x, now_pos.y, zorder);

        
        //if (Calc_length(begin_touch, end_touch) < 0.5 * interval)
        //    return;
        Calc_angle();


    }

    private void OnMouseDrag()
    {
        now_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        //this.transform.position = new Vector3(now_touch.x, now_touch.y, zorder);
        //Debug.Log(now_pos);
        //Debug.Log(now_touch);

    }
    private float Calc_length(Vector2 a, Vector2 b)
    {
        return (b - a).magnitude;
    }

    private void Calc_angle()
    {//tan 특성상에 맞추어 좌우 구분 필요,4방향
        //
        Vector2 direction = end_touch - begin_touch;
        if(direction.magnitude<interval/ change_sensitivity)
        {
            //Debug.Log("too short to change");
            return;

        }
        float dir_angle = Mathf.Atan(direction.y / direction.x);
        if (end_touch.x - begin_touch.x > 0)//우측
        {
            if (dir_angle < -1)
                now_dir = dir_4.S;//Debug.Log("S");
            else if (dir_angle < 1)
                now_dir = dir_4.E;//Debug.Log("E");
            else
                now_dir = dir_4.N;//Debug.Log("N");
        }
        else
        {
            if (dir_angle < -1)
                now_dir = dir_4.N;//Debug.Log("N");
            else if (dir_angle < 1)
                now_dir = dir_4.W;//Debug.Log("W");
            else
                now_dir = dir_4.S;//Debug.Log("S");
        }
        //Debug.Log(now_dir);

        Check_end();
        Request_change(now_dir);
    }

    private void Translate_index(dir_4 in_dir) {
        switch (in_dir)
        {
            case dir_4.N:
                next_x = ind_x;
                next_y = ind_y - 1;
                break;
            case dir_4.S:
                next_x = ind_x;
                next_y = ind_y + 1;
                break;
            case dir_4.W:
                next_x = ind_x - 1;
                next_y = ind_y;
                break;
            case dir_4.E:
                next_x = ind_x + 1;
                next_y = ind_y;
                break;
            default: break;
        }

    }

    private dir_4 Opposite_dir(dir_4 in_dir)
    {
        dir_4 out_dir = 0;
        switch (in_dir)
        {
            case dir_4.N:
                out_dir = dir_4.S;
                break;
            case dir_4.S:
                out_dir = dir_4.N;
                break;
            case dir_4.W:
                out_dir = dir_4.E;
                break;
            case dir_4.E:
                out_dir = dir_4.W;
                break;
            default: break;
        }
        return out_dir;

    }

    private void Begin_Lerp_change()
    {
        change_dir = other_pos - now_pos;
        is_change = true;
    }

    private void Refresh_after_change()
    {
        is_change = false;
        this.gameObject.name = name_to_change;
        its_name = this.gameObject.name;
        this.transform.position = other_pos;
        now_pos = other_pos;
        ind_x = next_x;
        ind_y = next_y;
        Check_end();


    }

    private void Lerp_change()
    {
        this.transform.Translate(change_dir*Time.deltaTime*change_spd);
        //Debug.Log(this.transform.position);

        if ((this.transform.position - other_pos).magnitude < 0.1f)
        {
            Refresh_after_change();
        }
    }

    public bool Response_change(string prev, dir_4 prev_dir)//바꿔도 되는지 응답,이전 멤버 명칭 포함
    {
        bool out_bool = true;//조건
        if (out_bool)
        {
            Translate_index(prev_dir);
            name_to_change = prev;
            other_pos = GameObject.Find(prev).GetComponent<Candy>().Get_pos();
            Begin_Lerp_change();
        }
        return out_bool;
    }

    //private void Exchange()
    //{
    //    Debug.Log(this.gameObject.name + " will be exchanged");
    //}

    private void Request_change(dir_4 direction)
    {
        //
        //GameObject.Find(neighbors(int)now_dir)
        if (movablity[(int)direction] != true)
            return;
        if (GameObject.Find(neighbors[(int)now_dir]).GetComponent<Candy>().Response_change(its_name, Opposite_dir(direction)) != true)
            return;

        next_x = GameObject.Find(neighbors[(int)now_dir]).GetComponent<Candy>().Get_ind_x();
        next_y = GameObject.Find(neighbors[(int)now_dir]).GetComponent<Candy>().Get_ind_y();
        name_to_change = neighbors[(int)now_dir];
        other_pos = GameObject.Find(neighbors[(int)now_dir]).GetComponent<Candy>().Get_pos();
        Begin_Lerp_change();
        //Debug.Log(this.gameObject.name + " will be changed");
    }

    public void Init(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interv, int i, int j,string input_name)
    {
        //Debug.Log(candy_type);
        //Debug.Log(Candies[candy_type]);
        neighbors = new string[4];
        movablity = new bool[4];
        this.its_name = input_name;
        //allow_change = true;//for debug
        height = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_height;
        width = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_width;
        this.ind_y = j;
        this.ind_x = i;
        this.interval = interv;
        this.this_type = candy_type - 2;//0과 1인거 제외하기위해 보정
        stay_range *= interval;
        accel_factor *= 0.001f;
        Stop_anim_power *= 0.1f;

        its_Candy = Instantiate(Candies[this_type], now_pos, Quaternion.identity) as GameObject;
        scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;
        its_Candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
        its_Candy.transform.SetParent(this.transform);
        its_Candy.transform.Translate(0, 0, zorder);
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;

        this.now_pos = new Vector3(begin_pos.x, begin_pos.y,zorder);
        //this.transform.position = begin_pos
        this.dest_pos = new Vector3(target_pos.x, target_pos.y, zorder);
        this.transform.position = dest_pos + new Vector3(0, queues * interval,0);
        this.is_lerp = true;
        //Debug.Log(now_pos);
        //Debug.Log(dest_pos);
    }
    //ready to bomb
    public bool Check_by_dir(dir_4 in_dir, ref int score)
    {

        if (!movablity[(int)in_dir]) {
            Debug.Log("i could not move");
            return false;
        }

        if (GameObject.Find(neighbors[(int)in_dir]).GetComponent<Candy>().Are_u_same(this_type))
        {
            //Debug.Log("Candytype:" + this_type);
            score++;
            GameObject.Find(neighbors[(int)in_dir]).GetComponent<Candy>().Check_by_dir(in_dir, ref score);
            return true;
        }
        return false;
    }

    public bool Check_all_dir(ref int r_score, ref int c_score)
    {

        for (int i = 0; i < 2; i++)
        {
            //Debug.Log((dir_4)i);
            if (movablity[i])
            {
                if (GameObject.Find(neighbors[i]).GetComponent<Candy>().Are_u_same(this_type))
                    GameObject.Find(neighbors[i]).GetComponent<Candy>().Check_all_dir(ref r_score, ref c_score);
            }
            if (movablity[i + 2])
            {
                if (GameObject.Find(neighbors[i+2]).GetComponent<Candy>().Are_u_same(this_type))
                    GameObject.Find(neighbors[i+2]).GetComponent<Candy>().Check_all_dir(ref r_score, ref c_score);
            }
        }

        //if (GameObject.Find(neighbors[(int)in_dir]).GetComponent<Candy>().Are_u_same(this_type))
        //{
        //    //Debug.Log("Candytype:" + this_type);
        //    score++;
        //    GameObject.Find(neighbors[(int)in_dir]).GetComponent<Candy>().Check_by_dir(in_dir, ref score);
        //    return true;
        //}
        return false;
    }

    public bool Are_u_same(int candy_type)
    {
        //Debug.Log("my:" + this_type + ", you: " + candy_type);
        if (candy_type != this_type)
            return false;
        return true;
    }

    private void Check_score()
    {
        Check_end();

        row_score = 0;
        col_score = 0;
        for (int i = 0; i < 2; i++)
        {
            //Debug.Log((dir_4)i);
            if(movablity[i])
                Check_by_dir((dir_4)i, ref col_score);

            if (movablity[i+2])
                Check_by_dir((dir_4)(i+2), ref row_score);
        }
        Debug.Log("row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));


        //if (GameObject.Find(neighbors[i]).GetComponent<Candy>().Are_u_same(this_type))//ns
        //{
        //    col_score++;
        //}
        //if (GameObject.Find(neighbors[i]).GetComponent<Candy>().Are_u_same(this_type))//we
        //{

        //}

        //for (int i = 2; i < 4; i++)
        //{
        //}
        //if (GameObject.Find(neighbors[(int)dir_4.N]).GetComponent<Candy>().Are_u_same(this_type))
        //{

        //}
    }
	// Use this for initialization
	void Start () {
		
	}

    private void Check_end()
    {
        movablity = new bool[4];
        neighbors = new string[4];
        //string name_find = "Candy[" + ind_x + "][" + (ind_y - 1) + "]";
        if (GameObject.Find("Candy[" + ind_x + "][" + (ind_y - 1) + "]") != null)//N
        {
            movablity[0] = true;
            neighbors[0] = "Candy[" + ind_x + "][" + (ind_y - 1) + "]";
            //Candy temp = GameObject.Find("Candy[" + ind_x + "][" + (ind_y - 1) + "]").GetComponent<Candy>();
        }
        else
            movablity[0] = false;
        //S
        if (GameObject.Find("Candy[" + ind_x + "][" + (ind_y + 1) + "]") != null)//S
        {
            movablity[1] = true;
            neighbors[1] = "Candy[" + ind_x + "][" + (ind_y + 1) + "]";
        }
        else
            movablity[1] = false;
        //W
        if (GameObject.Find("Candy[" + (ind_x - 1) + "][" + ind_y + "]") != null)//W
        {
            movablity[2] = true;
            neighbors[2] = "Candy[" + (ind_x - 1) + "][" + ind_y + "]";
        }
        else
            movablity[2] = false;
        //E
        if (GameObject.Find("Candy[" + (ind_x + 1) + "][" + ind_y + "]") != null)//E
        {
            movablity[3] = true;
            neighbors[3] = "Candy[" + (ind_x + 1) + "][" + ind_y + "]";
        }
        else
            movablity[3] = false;
        //        Debug.Log((int)now_dir);
        //          Debug.Log("Candy[" + (ind_x + 1) + "][" + ind_y + "]");
        //        Debug.Log(movablity[(int)now_dir]);
        //        for (int i = 0; i < 4; i++)
        //        {
        //            Debug.Log("movablity of " + (dir_4)i + ":" + movablity[i].ToString());
        //        }
        //Debug.Log("movablity of " + now_dir + ":" + movablity[(int)now_dir].ToString());
    }

    //private bool Check_end(int x, int y, int direction)
    //{
    //    switch (direction)
    //    {
    //        case 0:
    //            string name_find = "Candy[" + x + "][" + (y-1) + "]";
    //            if (GameObject.Find(name_find) == null)
    //                movablity[0] = true;

    //            break;

    //    }
    //}

    private void Find_neighbors()
    {
        //string N_name="Candy[" + i + "][" + j + "]";
        //      N:0, 
        //  W:2      E:3
        //      S:1
        //for (int ind = 0;ind<4)
        
    }

	//private void Init_lerp()
 //   {
 //       spd = 0.000f;
 //   }


    private void Stop_anim()
    {
        if (Stop_anim_time > Stop_anim_length)
            is_stop_anim = false;
        Stop_anim_time += Time.deltaTime;
        its_Candy.transform.localScale = new Vector2(1 + Stop_anim_power * Mathf.Sin(Mathf.PI/2*Stop_anim_spd * Stop_anim_time), 
            1 - Stop_anim_power * Mathf.Sin(Mathf.PI / 2 * Stop_anim_spd * Stop_anim_time)) * scale_factor;
        its_Candy.transform.position = now_pos - new Vector3(0, scale_factor - its_Candy.transform.localScale.y, 0);//new Vector3(now_pos.x,now_pos.y,zorder) - new Vector3(0,scale_factor - its_Candy.transform.localScale.y,0);
        //Debug.Log(scale_factor);
        //Debug.Log(transform.localScale.y);
    }
    private void End_lerp()
    {
        
        spd = 0.000f;
        is_lerp = false;
        this.now_pos = dest_pos;
        this.transform.position = dest_pos;
        is_stop_anim = true;
    }
    private void Lerp_move()
    {
        //Debug.Log("im moving now....");
        spd += accel_factor*Time.deltaTime;
        if (spd > max_spd)
            spd = max_spd;
        
        this.transform.Translate(new Vector2(0, -spd));
        //Debug.Log(this.transform.position);
        if (this.transform.position.y <= dest_pos.y + 0.1f)
            End_lerp();
        if (!is_show && this.transform.position.y < now_pos.y)
            its_Candy.GetComponent<SpriteRenderer>().enabled = true;
    }
    //public Vector2 Get_pos()
    //{
    //    return now_pos;
    //}
    public Vector3 Get_pos()
    {
        return now_pos;//new Vector3(now_pos.x, now_pos.y,zorder);
    }

    public int Get_ind_x()
    {
        return ind_x;
    }

    public int Get_ind_y()
    {
        return ind_y;
    }

    public string Get_name()
    {
        return this.gameObject.name;
    }

    // Update is called once per frame
    void Update () {
        if (is_lerp)
        {
            Lerp_move();
            return;
        }
        if (is_stop_anim)
            Stop_anim();
        if (is_change)
            Lerp_change();
	}
}
