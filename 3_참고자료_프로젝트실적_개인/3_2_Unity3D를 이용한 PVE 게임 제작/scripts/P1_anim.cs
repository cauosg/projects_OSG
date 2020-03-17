//캐릭터 state에 따라 지정된 애니메이션을 재생합니다
//애니메이션은 미리 blend되어 있으며 unity에디터 내에서 확인 가능합니다

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
        }

    }


    public void FootR()
    {

    }
    public void FootL()
    {

    }
    public void Hit() { 

    }

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
            Stun();
            isstun = true;
            return;
        }

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


        isGrounded = GameObject.Find("P1Root").GetComponent<P1>().isGround();
        isdash = GameObject.Find("P1Root").GetComponent<P1>().isDashing();


        if (tleft && lenrg < maxenrg && IsNonAtk())
        {

            lenrg += increase;
            transform.rotation = Quaternion.Slerp(transform.rotation, Left, 10 * maxenrg * Time.deltaTime);
        }

        
        if (tright && renrg < maxenrg && IsNonAtk())
        {
            renrg += increase;
            transform.rotation = Quaternion.Slerp(transform.rotation, Right, 10 * maxenrg * Time.deltaTime);
        }

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
        
        
        if (lenrg < animlimit && renrg < animlimit)
            Stop();


        if (lenrg > 0)
        {
            

            lenrg -= decrease;
            Move();
            if (lenrg > animlimit)
                Stop();
            
        }
        if (renrg > 0)
        {

            renrg -= decrease;
            Move();
            if (renrg > animlimit)
                Stop();
        }
 
        if (uenrg > 0)
        {
            uenrg -= decrease * 10;
            animator.SetFloat("Move", 1f, 0.1f, Time.deltaTime*2.5f);
        }

        if (!isGrounded)
        {

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

            animator.Play("Jump");
        }
    }
    private void Dash()
    {
        if (IsNonAtk())
        {
            animator.Play("Dash");

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
    }
    private void Stun()
    {
        animator.Play("Stun");
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

