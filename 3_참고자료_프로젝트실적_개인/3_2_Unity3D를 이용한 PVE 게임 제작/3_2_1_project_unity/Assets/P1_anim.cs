using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class P1_anim : MonoBehaviour
{
    public ParticleSystem efct;

    public float maxenrg = 1.000f;
    public float increase = 0.300f;
    public float decrease = 0.100f;
    public float jumpheight = 10.0f;
    public float dashmount = 5.0f;
    public float animlimit = 0.5f;
    public float atkspd = 1.0f;
    public float stuncooldown = 1.0f;
    private static bool isjump, tspace, tup, tdown, tleft, tright, tatk,isdash,isstun = false;
    private float renrg, lenrg, uenrg, atkcool,stuncool = 0.000f;
    private bool isGrounded = false;
    private bool isstunned, dashavail = false;

    Quaternion Right = Quaternion.identity;
    Quaternion Left = Quaternion.identity;
    // Use this for initialization


    Animator animator;
    

    void Start()
    {
        animator = GetComponent<Animator>();

        Right.eulerAngles = new Vector3(0, 90.0f, 0);
        Left.eulerAngles = new Vector3(0, -90.0f, 0);
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == "fallingrock")
        {
            stuncool = stuncooldown;
            //Debug.Log("Stun");
        }

    }
    //private void OnTriggerEnter(Collision col)

    //{

        
    //}

    public void FootR()
    {

    }
    public void FootL()
    {

    }
    public void Hit() { 

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyUp(KeyCode.Space))
            tspace = false;
        if (Input.GetKeyUp(KeyCode.UpArrow))
            tup = false;
        if (Input.GetKeyUp(KeyCode.DownArrow))
            tdown = false;
        if (Input.GetKeyUp(KeyCode.LeftArrow))
            tleft = false;
        if (Input.GetKeyUp(KeyCode.RightArrow))
            tright = false;
        if (Input.GetKeyUp(KeyCode.Q))
            tatk = false;

        if (stuncool > 0.0f)
        {
            stuncool -= Time.deltaTime;
            renrg = 0.000f;
            atkcool = 0.000f;
            uenrg = 0.000f;
            lenrg = 0.000f;
            tatk = false;
            isdash = false;
            //isjump = false;
            Stun();
            isstun = true;
            return;
        }
        // planet.transform.position;

        if (Input.GetKey(KeyCode.Space))
            tspace = true;

        if (Input.GetKey(KeyCode.UpArrow))
            tup = true;

        if (Input.GetKey(KeyCode.DownArrow))
            tdown = true;

        if (Input.GetKey(KeyCode.LeftArrow))
            tleft = true;

        if (Input.GetKey(KeyCode.RightArrow))
            tright = true;

        if (Input.GetKey(KeyCode.Q))
            tatk = true;


        // 0,1,0 = up
        // 1,0,0 = right
        //if (tspace)
        //    transform.Translate(Vector3.up * fwdspeed * Time.deltaTime, Space.Self);
        isGrounded = GameObject.Find("P1Root").GetComponent<P1>().isGround();
        isdash = GameObject.Find("P1Root").GetComponent<P1>().isDashing();
        //Stop();
        //if (isGrounded && tspace && uenrg < maxenrg)
        //if (!isjump&&!isGrounded && uenrg<maxenrg)
        //{

        //    isjump = true;
        //    uenrg += jumpheight;
        //    dashavail = true;
        //    //isGrounded = false;

        //}
        //if (isGrounded)
        //    isjump = false;
        //Debug.Log(isGrounded);
        //Debug.Log(tspace);
        //Debug.Log(isGrounded);
        //if (tdown && denrg < maxenrg)
        //    transform.Rotate(Vector3.right, -speed, Space.Self);

        if (tleft && lenrg < maxenrg && IsNonAtk())
        {

            lenrg += increase;
            transform.rotation = Quaternion.Slerp(transform.rotation, Left, 10 * maxenrg * Time.deltaTime);
        }

        
        if (tright && renrg < maxenrg && IsNonAtk())
        {
            //Debug.Log("rightrun");
            renrg += increase;
            transform.rotation = Quaternion.Slerp(transform.rotation, Right, 10 * maxenrg * Time.deltaTime);
        }

        //dash
        if (dashavail&&!isGrounded && uenrg < 1 && tleft && Input.GetKeyDown(KeyCode.Space) && IsNonAtk())
        {
            lenrg += dashmount;
            isdash = true;
            renrg = 0;
            dashavail = false;
        }


        if (dashavail && !isGrounded && uenrg < 1 && tright && Input.GetKeyDown(KeyCode.Space) && IsNonAtk())
        {
            renrg += dashmount;
            lenrg = 0;
            isdash = true;
            dashavail = false;
            
        }
        

        // animator.SetFloat("Move", 1f, 0.1f, Time.deltaTime);

        if (lenrg < animlimit && renrg < animlimit)
            Stop();


        if (lenrg > 0)
        {
            
            
            //if(transform.rotation.y < 90.0f*Mathf.Deg2Rad)
            //    transform.Rotate(Vector3.up, -lenrg, Space.Self);
            lenrg -= decrease;
            Move();
            if (lenrg > animlimit)
                Stop();
            
        }
        if (renrg > 0)
        {
            
           
            //    if (transform.rotation.y > -90.0f * Mathf.Deg2Rad)
            //        transform.Rotate(Vector3.up, renrg, Space.Self);
            renrg -= decrease;
            Move();
            if (renrg > animlimit)
                Stop();
        }
 
        if (uenrg > 0)
        {
           // transform.Translate(Vector3.up * uenrg * Time.deltaTime, Space.Self);
            uenrg -= decrease * 10;
            animator.SetFloat("Move", 1f, 0.1f, Time.deltaTime*2.5f);
        }

        //if (!isGrounded)
        //    Jump();
        if (!isGrounded)
        {
            //if (uenrg < 5f&&uenrg>0f)
            //    Land();
            //else
            if (isdash)
                Dash();
            else
                Jump();
        }
        else
            isdash = false;

        if (tatk)
            atkcool = 1.0f;


        if (atkcool > 0)
        {
            animator.speed = atkspd;
            Attack();
            atkcool -= Time.deltaTime * atkspd*2.5f;
        }
        else
        {
            animator.speed = 1.0f;
            atkcool = 0;
            efct.Stop();
        }
        //Debug.Log(atkcool);

    

    }
    private void Move()
    {
        if (IsNonAtk())
        {
            if (isGrounded)
                animator.Play("Stopped");
            animator.SetFloat("Move", 1f, 0.1f, Time.deltaTime);
        }
    }
    private void Jump()
    {
        if (IsNonAtk())
        {
            //animator.SetFloat("Move", 0f, 1f, Time.deltaTime);
            //animator.SetFloat("Jump", 1f, 0.1f, Time.deltaTime);
            animator.Play("Jump");
        }
    }
    private void Dash()
    {
        if (IsNonAtk())
        {
            animator.Play("Dash");
            
            //animator.SetFloat("Jump", 0.5f);
        }
    }
    private void Stop()
    {
        if (IsNonAtk())
        {
            if (isGrounded)
                animator.Play("Stopped");
            animator.SetFloat("Move", 0f, 0.1f, Time.deltaTime);
            animator.SetFloat("Jump", 0f, 0.1f, Time.deltaTime);
        }
    }
    private void Attack()
    {

        efct.Play();
        animator.Play("Attack1");
        //animator.SetFloat("Move", 0f, 0.1f, Time.deltaTime);
    }
    private void Stun()
    {
        animator.Play("Stun");
        //animator.SetFloat("Move", 0f, 0.1f, Time.deltaTime);
    }

    public bool IsNonAtk()
    {
        if (atkcool < 0.0001)
            return true;
        else
            return false;
    }
    public bool IsStunned()
    {
        if (stuncool > 0)
            return true;
        else
            return false;
    }
}

