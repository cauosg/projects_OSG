using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy : MonoBehaviour {

    public GameObject[] Candies;


    public float accel_factor = 0.00f;//1000으로 나눠줄 예정
    public float max_spd = 0.2f;
    public float zorder = -0.2f;
    public float Stop_anim_length = 1.0f;
    public float Stop_anim_spd = 1.0f;
    public float Stop_anim_power = 0.2f;//10으로 나눔
    public float stay_range = 0.2f;
    public float change_spd = 1.0f;
    public float change_sensitivity = 4;

    public bool ready_to_bomb = false;// { get { return ready_to_bomb; } set { ready_to_bomb = value; } }

    private GameObject its_Candy;
    private int this_type;
    private bool is_lerp, update_neighbors,is_show,is_stop_anim,is_moved,allow_change,is_change,check_once = false;
    private float spd = 0.000f;
    private float Stop_anim_time = 0.00f;
    private float scale_factor = 0;
    private float interval;
    private bool is_update;
    private Candy_dispenser dispenser;

    public enum dir_4 {N, S, W, E };
    public enum check_dir {row, col};
    private string[] neighbors;
    private string name_to_change;
    private dir_4 now_dir;
    private bool[] movablity;
    private int ind_y, ind_x, width, height, next_x, next_y;
    private Vector2 begin_touch,end_touch, now_touch;
    private Vector3 dest_pos, now_pos, other_pos, change_dir;
    private int row_score, col_score;

    private List<string> my_powder;
    //private List<string> bullet_powder;
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
        //enum Oppo{S,N,E, W}
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

    //private void End_lerp()
    //{

    //    spd = 0.000f;
    //    is_lerp = false;
    //    this.now_pos = dest_pos;
    //    this.transform.position = dest_pos;
    //    is_stop_anim = true;
    //}

    private void Refresh_after_change()
    {
        is_change = false;
        //this.gameObject.name = name_to_change;
        this.transform.position = other_pos;
        now_pos = other_pos;
        //ind_x = next_x;
        //ind_y = next_y;
        Check_end();
        Check_score();
        Change_is_drop(false);
    }

    private void Lerp_change()
    {
        this.transform.Translate(change_dir*Time.deltaTime*change_spd);
        //Debug.Log(this.transform.position);
        Change_is_drop(true);
        if ((this.transform.position - other_pos).magnitude < 0.1f)
        {
            Refresh_after_change();
        }
    }

    private void Transforming()
    {
        this.gameObject.name = name_to_change;
        //this.transform.position = other_pos;
        ind_x = next_x;
        ind_y = next_y;
    }

    public bool Response_change(int i, int j, dir_4 prev_dir, Vector3 my_pos)//바꿔도 되는지 응답,이전 멤버 명칭 포함
    {
        bool out_bool = true;//조건
        if (out_bool)
        {
            Translate_index(Opposite_dir(prev_dir));
            name_to_change = Name_translate(i, j);
            next_x = i;
            next_y = j;

            Transforming();

            other_pos = my_pos;
            Begin_Lerp_change();
        }
        return out_bool;
    }


    private void Request_change(dir_4 direction)
    {
        //
        //GameObject.Find(neighbors(int)now_dir)
        if (!movablity[(int)direction])
            return;

        Candy next_candy = GameObject.Find(neighbors[(int)now_dir]).GetComponent<Candy>();
        if (next_candy.Response_change(ind_x, ind_y, direction, now_pos) != true)
            return;

        Translate_index(direction);
        name_to_change = neighbors[(int)now_dir];
        Transforming();
        other_pos = next_candy.Get_pos();
        Begin_Lerp_change();
        //Debug.Log(this.gameObject.name + " will be changed");
    }

    private void Begin_drop(Vector2 begin_pos, Vector2 target_pos, int qs)
    {
        this.now_pos = new Vector3(begin_pos.x, begin_pos.y, zorder);
        //this.transform.position = begin_pos
        this.dest_pos = new Vector3(target_pos.x, target_pos.y, zorder);
        this.transform.position = dest_pos + new Vector3(0, qs * interval, 0);
        this.is_lerp = true;
    }

    private string Name_translate(int i, int j)
    {
        return "Candy[" + i + "][" + j + "]";
    }

    public void Init(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interv, int i, int j, int queues)
    {
        //Debug.Log(candy_type);
        //Debug.Log(Candies[candy_type]);

        dispenser = GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>();
        gameObject.name = Name_translate(i,j);
        //allow_change = true;//for debug
        height = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_height;
        width = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_width;
        this.ind_y = j;
        this.ind_x = i;
        this.interval = interv;
        this.this_type = candy_type - 2;//0과 1인거 제외하기위해 보정
        stay_range *= interval;


        its_Candy = Instantiate(Candies[this_type], now_pos, Quaternion.identity) as GameObject;
        scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;
        its_Candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
        its_Candy.transform.SetParent(this.transform);
        its_Candy.transform.Translate(0, 0, zorder);
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;

        Begin_drop(begin_pos, target_pos, queues);
    }

    //private void bullet_trigger(string item)
    //{
    //    if (!dispenser.powders.Contains(item))
    //        dispenser.powders.Add(item);
    //}

    private void bullet_load(string item)
    {
        if (dispenser.powders.Contains(item))
            return;
        if (my_powder.Contains(item))
            return;

        my_powder.Add(item);
    }


    private void bullet_trigger()
    {
        //if (!dispenser.powders.Contains(item))
        //    dispenser.powders.Add(item);
        dispenser.powders.AddRange(my_powder);
    }

    public bool Check_all_dir(ref int row_score, ref int col_score)//,override
    {
        for (int i = 0; i < 4; i++)
        {
            GameObject neighbor_candy = GameObject.Find(neighbors[i]);

            if (neighbor_candy == null)
                continue;

            //Debug.Log(this.gameObject.name + "candy will check " + neighbors[i]);

            if (!neighbor_candy.GetComponent<Candy>().Are_u_same(this_type))
                continue;

            //Debug.Log(this.gameObject.name + "is same with  " + neighbor_candy.GetComponent<Candy>().gameObject.name + "by " + (dir_4)i);
            my_powder.Add(neighbors[i]);
            //bullet_trigger(neighbors[i]);

            if (i > 1)
                row_score++;
            else
                col_score++;
            neighbor_candy.GetComponent<Candy>().Check_all_dir(this_type,(dir_4)i, ref row_score, ref col_score, ref my_powder);
        }

        return false;
    }

    public bool Check_all_dir(int check_type, dir_4 prev_dir, ref int row_score, ref int col_score, ref List<string> parent_powder)//, ref int org_type)
    {
        dir_4 except_dir = Opposite_dir(prev_dir);
        //Debug.Log("check is operated in" + this.gameObject.name);
        
        for (int i = 0; i < 4; i++)
        {
            //Debug.Log((dir_4)i);
            if (i == (int)except_dir)
            {
                //Debug.Log(this.gameObject.name + "'s skipped " + (dir_4)i);
                continue;
            }

            GameObject neighbor_candy = GameObject.Find(neighbors[i]);

            if (neighbor_candy == null)
                continue;

            //Debug.Log(this.gameObject.name + "candy will check " + neighbors[i]);

            if (!neighbor_candy.GetComponent<Candy>().Are_u_same(check_type))
                continue;

            //Debug.Log(this.gameObject.name + "is same with  " + neighbor_candy.GetComponent<Candy>().gameObject.name + "by " + (dir_4)i);

            //bullet_trigger(neighbors[i]);
            parent_powder.Add(neighbors[i]);

            if (i > 1)
                row_score++;
            else
                col_score++;

            neighbor_candy.GetComponent<Candy>().Check_all_dir(check_type,(dir_4)i, ref row_score, ref col_score, ref parent_powder);
            //if (i > 1)//2,3
            //    score++;
            //if (i < 2)//0,1
            //    score++;
        }

        return false;
    }

    public bool Are_u_same(int candy_type)
    {
        //Debug.Log("my:" + this_type + ", you: " + candy_type);

        if (candy_type != this_type || ready_to_bomb)
            return false;
        //if(!ready_to_bomb)
        ready_to_bomb = true;
        return true;
        
    }

    private void Check_score()
    {
        //Check_end();
        
        row_score = 1;
        col_score = 1;
        Check_all_dir(ref row_score, ref col_score);

        Debug.Log(gameObject.name + "'s row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));

        if (row_score > 2 || col_score > 2)
        {
            //dispenser.fired = true;

            bullet_load(gameObject.name);
            bullet_trigger();
            dispenser.Explode_candies();
        }
        my_powder.Clear();
    }
    public void Bomb()
    {

        GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Refill_plz(ind_x, ind_y);

        Destroy(this.gameObject);
    }
    //private void Bomb_chain()
    //{
    //    //if (!ready_to_bomb)
    //    //    return;
    //    for(int i = 0; i< bullet_powder.Count; i++)
    //    {
    //        if (dispenser.powders.Contains(bullet_powder[i]))
    //            continue;
    //        GameObject.Find(bullet_powder[i]).GetComponent<Candy>().Bomb_chain();
    //        dispenser.powders.Add(bullet_powder[i]);
    //    }
    //    //Debug.Log("Destroyed, myname:" + this.gameObject.name);
    //    GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Refill_plz(ind_x, ind_y);
    //    //GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>().Candy_drop();
    //    //Debug.Log("request is succeed [" + ind_x + "][" + ind_y + "]");
    //    Destroy(this.gameObject);
    //}

	// Use this for initialization
	void Start () {
        //bullet_powder = new List<string>();
        neighbors = new string[4];
        movablity = new bool[4];
        my_powder = new List<string>();
        accel_factor *= 0.001f;
        Stop_anim_power *= 0.1f;
    }

    private void Check_end()
    {
        movablity = new bool[4];
        neighbors = new string[4];
        ready_to_bomb = false;
        //string name_find = "Candy[" + ind_x + "][" + (ind_y - 1) + "]";
        //if (GameObject.Find("Candy[" + ind_x + "][" + (ind_y - 1) + "]") != null)//N
        //N
        string now_name = Name_translate(ind_x, ind_y - 1);
        if (GameObject.Find(now_name) != null)//N
        {
            movablity[0] = true;
            neighbors[0] = now_name;
        }
        else
            movablity[0] = false;
        //S
        now_name = Name_translate(ind_x, ind_y + 1);
        if (GameObject.Find(now_name) != null)//S
        {
            movablity[1] = true;
            neighbors[1] = now_name;
        }
        else
            movablity[1] = false;
        //W
        now_name = Name_translate(ind_x - 1, ind_y);
        if (GameObject.Find(now_name) != null)//W
        {
            movablity[2] = true;
            neighbors[2] = now_name;
        }
        else
            movablity[2] = false;
        //E
        now_name = Name_translate(ind_x + 1, ind_y);
        if (GameObject.Find(now_name) != null)//E
        {
            movablity[3] = true;
            neighbors[3] = now_name;
        }
        else
            movablity[3] = false;

        //bullet_powder.Clear();
    }

    public void Move_to_new(int i, int j, Vector3 term_pos, int steps)
    {
        Begin_drop(now_pos, term_pos,steps);
        
        gameObject.name = Name_translate(i,j);
        ind_x = i;
        ind_y = j;
    }


    private void Stop_anim()
    {
        Change_is_drop(true);
        if (Stop_anim_time > Stop_anim_length)
        {
            is_stop_anim = false;
            Check_end();
            Check_score();
            Change_is_drop(false);
        }
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
        
        //dispenser.is_update = true;
    }

    //private void End_lerp(string ntc)
    //{
    //    //this.gameObject.name = name_to_change;
    //    //ind_x = next_x;
    //    //ind_y = next_y;
    //    Check_end();

    //    Check_score();

    //    spd = 0.000f;
    //    is_lerp = false;
    //    this.now_pos = dest_pos;
    //    this.transform.position = dest_pos;
    //    is_stop_anim = true;

    //    Change_is_drop(false);
    //}


    private void Change_is_drop(bool in_bool)
    {
        if (dispenser.is_drop != in_bool)
            dispenser.is_drop = in_bool;
        is_update = false;
    }
    private void Lerp_move()
    {
        Change_is_drop(true);
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
        //if (ready_to_bomb)
        //    ready_to_bomb = false;
        if (dispenser.is_update)
        {
            //Debug.Log("now_checking");
            Check_end();
        }
	}
}
