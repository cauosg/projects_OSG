using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy : MonoBehaviour {

    public GameObject[] Candies;

    private GameObject its_Candy;
    private int this_type;
    private bool is_lerp, update_neighbors,is_show,is_stop_anim,is_moved = false;
    private float spd = 0.000f;
    private float Stop_anim_time = 0.00f;
    private float scale_factor = 0;
    public int queues = 7;
    public float accel_factor = 0.00f;//1000으로 나눠줄 예정
    public float max_spd = 0.2f;
    public float zorder = -0.2f;
    public float Stop_anim_length = 1.0f;
    public float Stop_anim_spd = 1.0f;
    public float Stop_anim_power = 0.2f;//10으로 나눔
    private Vector2 now_pos, dest_pos,begin_touch,end_touch,now_touch;
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
        begin_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
    }
    private void OnMouseUp()
    {
        end_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        if(!is_moved)
            this.transform.position = new Vector3(now_pos.x, now_pos.y, zorder);
    }

    private void OnMouseDrag()
    {
        now_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        this.transform.position = new Vector3(now_touch.x, now_touch.y, zorder);
        //Debug.Log(now_pos);
        //Debug.Log(now_touch);

    }

    public void Init(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interval)
    {
        //Debug.Log(candy_type);
        //Debug.Log(Candies[candy_type]);

        accel_factor *= 0.001f;
        Stop_anim_power *= 0.1f; 
        this.this_type = candy_type - 2;//0과 1인거 제외하기위해 보정

        its_Candy = Instantiate(Candies[this_type], now_pos, Quaternion.identity) as GameObject;
        scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;
        its_Candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
        its_Candy.transform.SetParent(this.transform);
        its_Candy.transform.Translate(0, 0, zorder);
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;

        this.now_pos = begin_pos;
        //this.transform.position = begin_pos
        this.dest_pos = target_pos;
        this.transform.position = dest_pos + new Vector2(0, queues * interval);
        this.is_lerp = true;
        //Debug.Log(now_pos);
        //Debug.Log(dest_pos);


    }

	// Use this for initialization
	void Start () {
		
	}

    private void Find_neighbors()
    {

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
        its_Candy.transform.position = new Vector3(now_pos.x,now_pos.y,zorder) - new Vector3(0,scale_factor - its_Candy.transform.localScale.y,0);
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

	// Update is called once per frame
	void Update () {
        if (is_lerp)
        {
            Lerp_move();
            return;
        }
        if (is_stop_anim)
            Stop_anim();
	}
}
